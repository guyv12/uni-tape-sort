#include "sort.hpp"

#include <cstdint>
#include <cmath>
#include <array>
#include <vector>
#include <algorithm>

void sort(std::filesystem::path& file_path)
{
    FileHandler file_handler(file_path);
    std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT> buffers;

    create_runs(file_handler, buffers);
    merge(file_handler, buffers);
}


void create_runs(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers)
{
    size_t cur_blk = 0, blk_count = file_handler.get_blkcount();
    
    while (cur_blk < blk_count)
    {
        size_t blks_to_read = std::min((size_t)BUFFER_COUNT, blk_count - cur_blk);

        // read data into the buffers
        for (size_t i = 0; i < blks_to_read; i++)
            if (!file_handler.read_block(buffers[i], cur_blk + i)) break;

        quick_sort(buffers, 0, (BUFFER_COUNT * BLOCKING_FACTOR) - 1);

        // write sorted series to the file
        for(size_t i = 0; i < blks_to_read; i++)
            if (!file_handler.write_block(buffers[i], cur_blk + i)) break;

        cur_blk += blks_to_read;
    }
}


void merge(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers)
{
    size_t cur_blk = 0, blk_count = file_handler.get_blkcount();
    
    // we use nth buffer for storing the merge result
    std::array<uint8_t, DISK_PAGE_SIZE> writer_buffer;

    while (cur_blk < blk_count)
    {
        // we fill n - 1 buffers with data, n is for result
        size_t blks_to_read = std::min((size_t)BUFFER_COUNT - 1, blk_count - cur_blk);

        // read data into the buffers
        for (size_t i = 0; i < blks_to_read; i++)
            if (!file_handler.read_block(buffers[i], cur_blk + i)) break;

        merge_runs(file_handler, buffers, blks_to_read);

        cur_blk += blks_to_read;
    }
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

void merge_runs(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, const size_t buff_to_merge)
{
    // // double *double_arr = reinterpret_cast<double*>(buffers[i].data());
    // // long double val = double_arr[0] * pow(double_arr[1], 2);

    // // TODO build min heap
    // struct BufferCursor 
    // {
    //     size_t buffer_idx;
    //     size_t record_idx;
    // };

    // std::vector<BufferCursor> cursors;
    // for (size_t i = 0; i < buff_to_merge - 1; i++)
    //     cursors.push_back({i, 0});

    // std::make_heap(cursors.begin(), cursors.end());

    // // write min into the last buffer
    // // rebuild with a new value
}
