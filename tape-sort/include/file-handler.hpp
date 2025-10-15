#pragma once

#ifndef file_handler_hpp
#define file_handler_hpp

#include "consts.hpp"
#include <cstdint>
#include <filesystem>
#include <array>


class FileHandler
{
    public:

        FileHandler(const std::filesystem::path& file_path);

        void read(std::array<uint8_t, DISK_PAGE_SIZE>& out, size_t block_idx);
        void write(uint8_t data);
        void flush();

        size_t get_blkcount();

        unsigned long long int get_reads();
        unsigned long long int get_writes();

        void print();

    private:
        
        std::filesystem::path file_path;

        std::array<uint8_t, DISK_PAGE_SIZE> block_buffer;
        size_t buffer_idx;

        size_t block_counter;

        unsigned long long int r_events;
        unsigned long long int w_events;
        
};


#endif //file_handler_hpp
