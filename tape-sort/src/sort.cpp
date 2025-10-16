#include "sort.hpp"

#include <cstdint>
#include <array>
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

    // sort each buffer
    for (size_t i = 0; i < buff_to_sort; i++)
        sort_run(buffers[i]);
}

void sort_run(std::array<uint8_t, DISK_PAGE_SIZE>& buffer)
{
    // build record files
    // quick_sort(NULL, 0, DISK_PAGE_SIZE - 1);
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

void merge_runs(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, const size_t buff_to_sort)
{
    // build min heap
    // write min into the last buffer
    // rebuild with a new value
}



// ------ miscellanous ------

void quick_sort(Record *arr, int l, int r)
{
    if (l >= r) return; // recursion check

    int q = partition(arr, l, r);
    quick_sort(arr, l, q);
    quick_sort(arr, q + 1, r);
}

int partition(Record *arr, int l, int r)
{
    long double pivot = arr[l].get_value();

    while (true)
    {
        while (arr[l].get_value() < pivot) l++;
        while (arr[r].get_value() > pivot) r--;

        if (l <  r) 
            std::swap(arr[l++], arr[r--]);

        else
            return r;
    }
}
