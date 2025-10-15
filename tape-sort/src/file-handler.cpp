#include "file-handler.hpp"


FileHandler::FileHandler(const std::filesystem::path& file_path) : file_path(file_path)
{
    if (!file_path.parent_path().empty())
        std::filesystem::create_directories(file_path.parent_path());

    FILE *file = std::fopen(file_path.c_str(), "ab");
    if (!file) { perror("File Handler: Can't open/create a FileHandler file"); return; }

    fseek(file, 0, SEEK_END); block_counter = ftell(file) / DISK_PAGE_SIZE;
    fclose(file);

    block_buffer.fill((uint8_t)'\0'); buffer_idx = 0;

    r_events = 0; w_events = 0;
}


void FileHandler::read(std::array<uint8_t, DISK_PAGE_SIZE>& out, size_t block_idx)
{
    out.fill('\0');

    if (block_idx + 1 > block_counter) return; // if the searched block is not present in the file
    
    FILE *file = fopen(file_path.c_str(), "rb");
    if (!file) { perror("File Handler: Can't open FileHandler file"); return; }

    fseek(file, DISK_PAGE_SIZE * block_idx, SEEK_SET);
    fread(out.data(), 1, DISK_PAGE_SIZE, file);
    fclose(file);

    r_events++;
}

void FileHandler::write(uint8_t data)
{
    block_buffer[buffer_idx++] = data;

    if (buffer_idx == DISK_PAGE_SIZE) flush();
}

void FileHandler::flush()
{
    if (buffer_idx == 0) return; // if there's nothing to write

    FILE *file = fopen(file_path.c_str(), "ab+");
    if (!file) { perror("File Handler: Can't open FileHandler file"); return; }

    fseek(file, 0, SEEK_END);
    fwrite(block_buffer.data(), 1, buffer_idx, file);
    fclose(file);

    block_buffer.fill((uint8_t)'\0'); buffer_idx = 0;

    block_counter++; w_events++;
}


size_t FileHandler::get_blkcount() { return block_counter; }

unsigned long long int FileHandler::get_reads() { return r_events; }
unsigned long long int FileHandler::get_writes() { return w_events; }


void FileHandler::print()
{
   // TODO Read, cast to record, print records, repeat
}
