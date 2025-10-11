#pragma once

#ifndef file_handler_hpp
#define file_handler_hpp

#include <filesystem>
#include <array>

#define BLOCK_SIZE 512 // size of the handled blocks in bytes


class FileHandler
{
    public:

        FileHandler(const std::filesystem::path& file_path);

        void read(std::array<char, BLOCK_SIZE>& out, size_t block_idx);
        void write(char data);
        void flush();

        size_t get_blkcount();

        unsigned long long int get_reads();
        unsigned long long int get_writes();

    private:
        
        std::filesystem::path file_path;

        std::array<char, BLOCK_SIZE> block_buffer;
        size_t buffer_idx;

        size_t block_counter;

        unsigned long long int r_events;
        unsigned long long int w_events;
        
};


#endif //file_handler_hpp
