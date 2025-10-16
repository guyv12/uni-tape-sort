#pragma once

#ifndef sort_hpp
#define sort_hpp

#include "consts.hpp"
#include "record.hpp"
#include "file-handler.hpp"
#include <filesystem>


void sort(std::filesystem::path& file_path);

// algorithm
void create_runs(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers);
void merge(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers);

// individual file sort
void sort_runs(std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, const size_t buff_to_sort);
void sort_run(std::array<uint8_t, DISK_PAGE_SIZE>& buffer);

// buffer merging
void merge_runs(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers, const size_t buff_to_sort);

void quick_sort(Record *arr, int l, int r);
int partition(Record *arr, int l, int r);

#endif //sort_hpp
