#include "sort.hpp"

#include <cstring>
#include <cstdint>
#include <cmath>
#include <array>
#include <queue>
#include <algorithm>


void sort(const std::filesystem::path& file_path, bool verbose)
{
    FileHandler file_handler(file_path);
    std::array<Buffer, BUFFER_COUNT> buffers;
    RunInfo run_info;
    unsigned long long int rw;
    
    if (verbose) file_handler.print();

    int sort_phases = create_runs(file_handler, buffers, run_info, verbose);
    int merge_phases = merge(file_handler, buffers, run_info, rw, verbose);

    if (verbose) file_handler.print();

    printf("\nsort: %d | merge: %d\n", sort_phases, merge_phases);
    printf("r/w operations: %llu\n\n\n", rw);

    // FILE *data_file = fopen("sort.dat", "a");
    // fprintf(data_file, " %llu\n", rw);
    // fclose(data_file);
}


int create_runs(FileHandler& file_handler, std::array<Buffer, BUFFER_COUNT>& buffers, RunInfo& run_info, bool verbose)
{
    int phases = 0;

    size_t cur_blk = 0, blk_count = file_handler.get_blkcount();
    run_info.run_count = 0; run_info.run_size = BUFFER_COUNT;

    while (cur_blk < blk_count)
    {
        size_t blks_to_read = std::min((size_t)BUFFER_COUNT, blk_count - cur_blk);

        // read data into the buffers
        for (size_t i = 0; i < blks_to_read; i++)
        {
            size_t bytes = file_handler.read_block(buffers[i].array(), cur_blk + i); // <== here should be some check how many records are inside?
            buffers[i].set_size(bytes);
        }

        int last_offset = 0;
        for (size_t i = 0; i < blks_to_read; i++)
            last_offset += buffers[i].size() / RECORD_SIZE;
        quick_sort(buffers, 0, last_offset - 1);

        // write sorted series to the file
        for(size_t i = 0; i < blks_to_read; i++)
            file_handler.write_block(buffers[i].array(), cur_blk + i, buffers[i].size());

        if (verbose) file_handler.print();

        cur_blk += blks_to_read;
        run_info.run_count++;
        phases++;
    }

    return phases;
}


int merge(FileHandler& file_handler, std::array<Buffer, BUFFER_COUNT>& buffers, RunInfo& run_info, unsigned long long int& rw, bool verbose)
{
    int phases = 0;

    FileHandler tmp("tmp-db");
    bool toggle = false;

    while (run_info.run_count > 1)
    {
        FileHandler& in  = toggle ? tmp : file_handler;
        FileHandler& out = toggle ? file_handler : tmp;

        out.reset();
        merge_runs(in, out, buffers, run_info);

        if (verbose) out.print();
        
        toggle = !toggle;
        phases++;
    }

    rw = file_handler.get_reads() + file_handler.get_writes() + tmp.get_reads() + tmp.get_writes();

    if (toggle)
    {
        std::filesystem::remove(file_handler.get_file_path());
        std::filesystem::rename(tmp.get_file_path(), file_handler.get_file_path());
    }

    else
        std::filesystem::remove(tmp.get_file_path());

    return phases;
}


// ------ individual sort ------

long double get_local(std::array<Buffer, BUFFER_COUNT>& buffers, int global_ptr)
{
    // * 2 because 1 record = 2 doubles
    int ptr_buffer = global_ptr / BLOCKING_FACTOR, local_ptr = 2 * (global_ptr % BLOCKING_FACTOR);

    double *buffer_cast = reinterpret_cast<double *>(buffers[ptr_buffer].array().data());

    double m = buffer_cast[local_ptr], v = buffer_cast[local_ptr + 1]; // a whole record (2 doubles) is guaranteed to be in 1 buffer
    return Record::get_value(m, v);
}

void swap(std::array<Buffer, BUFFER_COUNT>& buffers, int global_l, int global_r)
{
    // * 2 because 1 record = 2 doubles
    int l_buffer = global_l / BLOCKING_FACTOR, l_ptr = 2 * (global_l % BLOCKING_FACTOR);
    int r_buffer = global_r / BLOCKING_FACTOR, r_ptr = 2 * (global_r % BLOCKING_FACTOR);

    double *l_cast = reinterpret_cast<double *>(buffers[l_buffer].array().data());
    double *r_cast = reinterpret_cast<double *>(buffers[r_buffer].array().data());

    std::swap(l_cast[l_ptr], r_cast[r_ptr]); // swap m
    std::swap(l_cast[l_ptr + 1], r_cast[r_ptr + 1]); // swap v
}


void quick_sort(std::array<Buffer, BUFFER_COUNT>& buffers, int global_l, int global_r)
{
    if (global_l >= global_r) return; // recursion check

    int q = partition(buffers, global_l, global_r);
    quick_sort(buffers, global_l, q);
    quick_sort(buffers, q + 1, global_r);
}

int partition(std::array<Buffer, BUFFER_COUNT>& buffers, int global_l, int global_r)
{
    long double pivot = get_local(buffers, global_l);


    while (true)
    {
        while (get_local(buffers, global_l) < pivot)
            global_l++;

        while (get_local(buffers, global_r) > pivot)
            global_r--;

        if (global_l <  global_r) 
            swap(buffers, global_l++, global_r--);

        else
            return global_r;
    }
}


//----- individual merge ----

void merge_runs(FileHandler& input, FileHandler& output, std::array<Buffer, BUFFER_COUNT>& buffers, RunInfo& run_info)
{
    // --- STRUCT DEF ---
    struct val_ptr 
    {
        double m;
        double v;

        int buffer;
    };

    struct RunCursor 
    {
        size_t run_start;       // starting block index
        size_t run_end;         // ending block index (last run can be smaller)
        size_t cur_block;       // current block index
        size_t buffer_offset;   // offset (in bytes not records!) within the current buffer
    };

    std::array<RunCursor, BUFFER_COUNT - 1> cursors;
    size_t output_ptr = 0;

    // create min heap
    auto cmp = [](const val_ptr& a, const val_ptr& b) 
    {
        return Record::get_value(a.m, a.v) > Record::get_value(b.m, b.v);
    };
    std::priority_queue<val_ptr, std::vector<val_ptr>, decltype(cmp)> heap(cmp);


    // -- MAIN MERGE LOOP ---
    size_t cur_blk = 0, blk_count = input.get_blkcount();
    int merged_total = 0, new_runs = 0;

    while (cur_blk < blk_count)
    {
        // --- INITIAL HEAP POPULATION ---
        size_t merged_runs = std::min(static_cast<size_t>(BUFFER_COUNT - 1), run_info.run_count - merged_total);

        // setup cursors and read the data into buffers
        for (int i = 0; i < merged_runs; i++)
        {
            cursors[i].run_start = cur_blk + i * run_info.run_size;
            cursors[i].run_end = std::min(blk_count, cursors[i].run_start + run_info.run_size);
            cursors[i].cur_block = cursors[i].run_start; cursors[i].buffer_offset = 0;

            size_t bytes = input.read_block(buffers[i].array(), cursors[i].run_start);
            buffers[i].set_size(bytes);
        }

        // convert the data to storeable in the heap, and store in the heap
        for (int i = 0; i < merged_runs; i++)
        {
            double *double_arr = reinterpret_cast<double *>(buffers[i].array().data());
            heap.push(val_ptr{ double_arr[0], double_arr[1], i });
        }


        // -- MERGE LOOP ---
        while(!heap.empty())
        {
            val_ptr min = heap.top(); heap.pop();

            // move the smallest record to the output buffer
            memcpy(buffers[BUFFER_COUNT - 1].array().data() + output_ptr,
                buffers[min.buffer].array().data() + cursors[min.buffer].buffer_offset,
                RECORD_SIZE
            );
            output_ptr += RECORD_SIZE; cursors[min.buffer].buffer_offset += RECORD_SIZE;

            // write if output full
            if (output_ptr + RECORD_SIZE > RECORD_BYTES)
            {
                output.write_block(buffers[BUFFER_COUNT - 1].array(), output.get_blkcount());
                output_ptr = 0;
            }

            // check if input exhausted
            if (cursors[min.buffer].buffer_offset + RECORD_SIZE > buffers[min.buffer].size())
            {
                cursors[min.buffer].cur_block++; cursors[min.buffer].buffer_offset = 0;

                // if run exceeded skip
                if (cursors[min.buffer].cur_block >= cursors[min.buffer].run_end)
                    continue;

                size_t bytes = input.read_block(buffers[min.buffer].array(), cursors[min.buffer].cur_block);
                buffers[min.buffer].set_size(bytes);
            }

            double *double_arr = reinterpret_cast<double *>(buffers[min.buffer].array().data() + cursors[min.buffer].buffer_offset);
            heap.push(val_ptr{ double_arr[0], double_arr[1], min.buffer });        
        }

        cur_blk += merged_runs * run_info.run_size;
        merged_total += merged_runs; new_runs++;
    }

    // write the last (possibly unfinished) part
    if (output_ptr != 0)
        output.write_block(buffers[BUFFER_COUNT - 1].array(), output.get_blkcount(), output_ptr);

    run_info.run_count = new_runs;
    run_info.run_size *= (BUFFER_COUNT - 1);
}


//----- helper ----

void check_if_sorted(const std::filesystem::path& file_path)
{
    bool sorted = true;

    FILE *checked_file = fopen(file_path.c_str(), "rb");
    if (!checked_file) { perror("sort: can't open file for check_if_sorted"); return; }

    long double prev_val = -std::numeric_limits<long double>::infinity();

    while(true)
    {
        double m, v;
        size_t read_m = fread(&m, sizeof(double), 1, checked_file);
        size_t read_v = fread(&v, sizeof(double), 1, checked_file);

        if (read_m != 1 || read_v != 1) break; // EOF

        long double new_val = Record::get_value(m, v);
        if (prev_val > new_val)
        { 
            sorted = false; 
            printf("\n!! FILE NOT SORTED AT: %d !!\n", ftell(checked_file) / RECORD_SIZE);
            printf("prev: %0.2Lf new: %0.2Lf\n", prev_val, new_val);
            break; 
        }
        prev_val = new_val;
    }

    if (sorted) printf("sort: OK - finished check at: %d\n", ftell(checked_file) / RECORD_SIZE);
    fclose(checked_file);
}


void print_buffers(std::array<Buffer, BUFFER_COUNT>& buffers)
{
    for (int i = 0; i < BUFFER_COUNT; i++)
    {
        double* arr = reinterpret_cast<double*>(buffers[i].array().data());

        for (int j = 0; j < BLOCKING_FACTOR; j++)
        {
            printf("%d: %0.2Lf ", i * BLOCKING_FACTOR + j, Record::get_value(arr[2 * j], arr[2 * j + 1]));
        }
        printf("\n");
    }
}
