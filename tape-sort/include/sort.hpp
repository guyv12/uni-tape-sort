#pragma once

#ifndef sort_hpp
#define sort_hpp

#include "consts.hpp"
#include "record.hpp"
#include "file-handler.hpp"

typedef struct RunInfo RunInfo;

struct RunInfo // run size will always be divisible by b -> as they are made from at least 1 buffer
{
    size_t run_count; // how many runs there are in total
    size_t run_size; // how many disk pages is each individual run (the last one can be less than that)
};


void sort(const std::filesystem::path& file_path);

// algorithm
void create_runs(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, RunInfo& run_info);
void merge(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, RunInfo& run_info);


// individual file sort
long double get_local(std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, int global_ptr);
void swap(std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, int global_l, int global_r);

void quick_sort(std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, int global_l, int global_r);
int partition(std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, int global_l, int global_r);


// buffer merging
void merge_runs(FileHandler& input, FileHandler& output,  std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, RunInfo& run_info);


// helper
void check_if_sorted(const std::filesystem::path& file_path);


#endif //sort_hpp
