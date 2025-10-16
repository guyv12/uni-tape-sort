#include "file-handler.hpp"


FileHandler::FileHandler(const std::filesystem::path& file_path) : file_path(file_path)
{
    if (!file_path.parent_path().empty())
        std::filesystem::create_directories(file_path.parent_path());

    FILE *file = std::fopen(file_path.c_str(), "ab");
    if (!file) { perror("File Handler: Can't open/create a FileHandler file"); return; }

    fseek(file, 0, SEEK_END); blk_counter = ftell(file) / DISK_PAGE_SIZE;
    fclose(file);

    r_events = 0; w_events = 0;
}


bool FileHandler::read_block(std::array<uint8_t, DISK_PAGE_SIZE>& out, const size_t block_idx)
/* @return true on success, false on failure */
{
    out.fill('\0');

    if (block_idx + 1 > blk_counter) return false; // if the idx not present
    
    FILE *file = fopen(file_path.c_str(), "rb");
    if (!file) { perror("File Handler: Can't open FileHandler file for read_block"); return false; }

    fseek(file, DISK_PAGE_SIZE * block_idx, SEEK_SET);
    fread(out.data(), 1, DISK_PAGE_SIZE, file);
    fclose(file);

    r_events++;
    return true;
}

bool FileHandler::write_block(const std::array<uint8_t, DISK_PAGE_SIZE>& data, size_t block_idx)
/* @return true on success, false on failure */
{
    FILE *file = fopen(file_path.c_str(), "rb+");
    if (!file) { perror("File Handler: Can't open FileHandler file for write_block"); return false; }

    if (block_idx + 1 > blk_counter)  block_idx = blk_counter++; // if idx not present, append at the end

    fseek(file, DISK_PAGE_SIZE * block_idx, SEEK_SET);
    fwrite(data.data(), 1, DISK_PAGE_SIZE, file);
    fclose(file);

    w_events++;
    return true;
}


size_t FileHandler::get_blkcount() { return blk_counter; }
unsigned long long int FileHandler::get_reads() { return r_events; }
unsigned long long int FileHandler::get_writes() { return w_events; }


void FileHandler::print()
{
   // TODO Read, cast to record, print records, repeat
}
