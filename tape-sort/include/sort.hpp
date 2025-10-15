#pragma once

#ifndef sort_hpp
#define sort_hpp

#include "consts.hpp"
#include "record.hpp"
#include "file-handler.hpp"
#include <filesystem>


void sort(std::filesystem::path& file_path);

void create_runs(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers);
void sort_run(std::array<uint8_t, DISK_PAGE_SIZE>& buffer);

void merge();

#endif //sort_hpp
