#include "sort.hpp"

#include <cstring>
#include <cstdint>
#include <cmath>
#include <array>
#include <queue>


void sort(std::filesystem::path& file_path)
{
    FileHandler file_handler(file_path);
    std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT> buffers;
    RunInfo run_info;

    create_runs(file_handler, buffers, run_info);
    merge(file_handler, buffers, run_info);
}


void create_runs(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, RunInfo& run_info)
{
    size_t cur_blk = 0, blk_count = file_handler.get_blkcount();
    run_info.run_size = BUFFER_COUNT;

    while (cur_blk < blk_count)
    {
        size_t blks_to_read = std::min((size_t)BUFFER_COUNT, blk_count - cur_blk);

        // read data into the buffers
        for (size_t i = 0; i < blks_to_read; i++)
            file_handler.read_block(buffers[i], cur_blk + i);

        quick_sort(buffers, 0, (BUFFER_COUNT * BLOCKING_FACTOR) - 1);

        // write sorted series to the file
        for(size_t i = 0; i < blks_to_read; i++)
            file_handler.write_block(buffers[i], cur_blk + i);

        cur_blk += blks_to_read;
        run_info.run_count++;
    }
}


void merge(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, RunInfo& run_info)
{
    
}


// ------ individual sort ------

long double get_local(std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, int global_ptr)
{
    // * 2 because 1 record = 2 doubles
    int ptr_buffer = global_ptr / BLOCKING_FACTOR, local_ptr = 2 * (global_ptr % BLOCKING_FACTOR);

    double *buffer_cast = reinterpret_cast<double *>(buffers[ptr_buffer].data());

    double m = buffer_cast[local_ptr], v = buffer_cast[local_ptr + 1]; // a whole record (2 doubles) is guaranteed to be in 1 buffer
    return Record::get_value(m, v);
}

void swap(std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, int global_l, int global_r)
{
    // * 2 because 1 record = 2 doubles
    int l_buffer = global_l / BLOCKING_FACTOR, l_ptr = 2 * (global_l % BLOCKING_FACTOR);
    int r_buffer = global_r / BLOCKING_FACTOR, r_ptr = 2 * (global_r % BLOCKING_FACTOR);

    double *l_cast = reinterpret_cast<double *>(buffers[l_buffer].data());
    double *r_cast = reinterpret_cast<double *>(buffers[r_buffer].data());

    std::swap(l_cast[l_ptr], r_cast[r_ptr]); // swap m
    std::swap(l_cast[l_ptr + 1], r_cast[r_ptr + 1]); // swap v
}


void quick_sort(std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, int global_l, int global_r)
{
    if (global_l >= global_r) return; // recursion check

    int q = partition(buffers, global_l, global_r);
    quick_sort(buffers, global_l, q);
    quick_sort(buffers, q + 1, global_r);
}

int partition(std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, int global_l, int global_r)
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

// void merge_runs(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, RunInfo& run_info)
// {
//     // read into buffers, and setup its pointers
//     struct val_ptr 
//     {
//         double m;
//         double v;

//         int buffer;
//     };

//     // these hold offsets in bytes not records
//     std::array<int, BUFFER_COUNT - 1> buff_pointers = { 0 }; // we use n - 1 buffers for storing the merge data
//     int output_ptr = 0;

//     FileHandler tmp("tmp-db");

//     // read data into the buffers
//     for (int i = 0; i < run_info.run_count; i++)
//         file_handler.read_block(buffers[i], i * run_info.run_size);

//     std::priority_queue<val_ptr> heap; // key = Record::get_value(m, v)? 

//     // convert the data to store in the heap and store in the heap
//     for (int i = 0; i < run_info.run_count; i++)
//     {
//         double *double_arr = reinterpret_cast<double *>(buffers[i].data());
//         heap.push(val_ptr{ double_arr[0], double_arr[1], i });
//     }

//     while(!heap.empty())
//     {
//         val_ptr min = heap.top(); heap.pop();

//         // move the smallest record to the output buffer
//         memcpy(buffers[BUFFER_COUNT - 1].data() + output_ptr, buffers[min.buffer].data() + buff_pointers[min.buffer], RECORD_SIZE);
//         output_ptr += RECORD_SIZE; buff_pointers[min.buffer] += RECORD_SIZE;

//         if (output_ptr + RECORD_SIZE >= DISK_PAGE_SIZE)
//         {
//             // write the buffer to the file
//             output_ptr = 0;
//         }


//         if (buff_pointers[min.buffer] + 1 < BLOCKING_FACTOR)
//         {
//             // read data again
//             buff_pointers[min.buffer] = 0;
//         }

//         //heap.push(val_ptr{ })
//     }
// }
