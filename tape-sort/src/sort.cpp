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

        sort_runs(buffers, blks_to_read);

        // write sorted series to the file
        for(size_t i = 0; i < blks_to_read; i++)
            if (!file_handler.write_block(buffers[i], cur_blk + i)) break;

        cur_blk += blks_to_read;
    }
}

void sort_runs(std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, const size_t buff_to_sort)
{
    // multithreaded?

    for (size_t i = 0; i < buff_to_sort; i++)
        quick_sort(buffers[i].data(), 0, DISK_PAGE_SIZE - 1);
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

void merge_runs(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, const size_t buff_to_merge)
{
    // double *double_arr = reinterpret_cast<double*>(buffers[i].data());
    // long double val = double_arr[0] * pow(double_arr[1], 2);

    // TODO build min heap
    struct BufferCursor 
    {
        size_t buffer_idx;
        size_t record_idx;
    };

    std::vector<BufferCursor> cursors;
    for (size_t i = 0; i < buff_to_merge - 1; i++)
        cursors.push_back({i, 0});

    std::make_heap(cursors.begin(), cursors.end());

    // write min into the last buffer
    // rebuild with a new value
}



// ------ miscellanous ------

void quick_sort(uint8_t *arr, int l, int r)
{
    if (l >= r) return; // recursion check

    int q = partition(arr, l, r);
    quick_sort(arr, l, q);
    quick_sort(arr, q + 1, r);
}

int partition(uint8_t *arr, int l, int r)
{
    double *double_arr = reinterpret_cast<double*>(arr);
    long double pivot = double_arr[l] * pow(double_arr[l + 1], 2);


    while (true)
    {
        while (double_arr[l] * pow(double_arr[l + 1], 2) < pivot)
            l += 2; // record is represented by 2 doubles

        while (double_arr[r] * pow(double_arr[r + 1], 2) > pivot)
            r -= 2;

        if (l <  r) 
        {
            std::swap(double_arr[l], double_arr[r]); // swap m
            std::swap(double_arr[l + 1], double_arr[r + 1]); // swap m

            l += 2; r -=2;
        }

        else
            return r;
    }
}
