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

        bool read_block(std::array<uint8_t, DISK_PAGE_SIZE>& out, const size_t block_idx);
        bool write_block(const std::array<uint8_t, DISK_PAGE_SIZE>& data, size_t block_idx);
        bool write_block(const std::array<uint8_t, DISK_PAGE_SIZE>& data, size_t block_idx, size_t bytes);

        void reset();
        std::filesystem::path get_file_path();
        
        size_t get_blkcount();
        unsigned long long int get_reads();
        unsigned long long int get_writes();

        void print();

    private:
        
        const std::filesystem::path file_path;

        size_t blk_counter;

        unsigned long long int r_events;
        unsigned long long int w_events;
        
};


#endif //file_handler_hpp
