#include "sort.hpp"

#include <cstdint>
#include <array>
#include <algorithm>

void sort(std::filesystem::path& file_path)
{
    FileHandler file_handler(file_path);
    std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT> buffers;

    create_runs(file_handler, buffers);

    
}


void create_runs(FileHandler& file_handler, std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT>& buffers)
{
    size_t cur_blk = 0, blk_count = file_handler.get_blkcount();
    
    while (cur_blk < blk_count)
    {
        size_t blks_to_read = std::min((size_t)BUFFER_COUNT, blk_count - cur_blk);

        // read data into the buffers
        for (size_t i = 0; i < blks_to_read; i++)
            file_handler.read(buffers[i], cur_blk + i);

        // sort each buffer
        for (size_t i = 0; i < blks_to_read; i++)
            sort_run(buffers[i]);

        // write sorted series to the file
        for(size_t i = 0; i < blks_to_read; i++)
        {
            for (uint8_t byte : buffers[i])
                file_handler.write(byte);
        }

        cur_blk += blks_to_read;
    }

    // to ensure all of the data is written to the file
    file_handler.flush();
}

void sort_run(std::array<uint8_t, DISK_PAGE_SIZE>& buffer)
{
    // TODO HeapSort

    // sorting placeholder
    Record* records = reinterpret_cast<Record*>(buffer.data());
    size_t record_count = DISK_PAGE_SIZE / sizeof(Record);

    std::sort(records, records + record_count,
              [](const Record& a, const Record& b) {
                  return a.get_value() < b.get_value();
              });
}

void merge()
{

}
