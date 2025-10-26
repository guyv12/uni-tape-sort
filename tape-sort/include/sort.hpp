#pragma once

#ifndef sort_hpp
#define sort_hpp

#include "consts.hpp"
#include "record.hpp"
#include "file-handler.hpp"


void sort(std::filesystem::path& file_path);

// algorithm
void create_runs(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers);
void merge(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers);


// individual file sort
long double get_local(std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, int global_ptr);
void swap(std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, int global_l, int global_r);

void quick_sort(std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, int global_l, int global_r);
int partition(std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, int global_l, int global_r);


// buffer merging
void merge_runs(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, const size_t buff_to_sort);

#endif //sort_hpp
