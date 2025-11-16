#include "file-handler.hpp"
#include "record.hpp"


FileHandler::FileHandler(const std::filesystem::path& file_path) : file_path(file_path)
{
    if (!file_path.parent_path().empty())
        std::filesystem::create_directories(file_path.parent_path());

    FILE *file = fopen(file_path.c_str(), "ab");
    if (!file) { perror("File Handler: Can't open/create a FileHandler file"); return; }

    fseek(file, 0, SEEK_END); blk_counter = (ftell(file) + RECORD_BYTES - 1) / RECORD_BYTES; // round up
    fclose(file);

    r_events = 0; w_events = 0;
}


size_t FileHandler::read_block(std::array<uint8_t, DISK_PAGE_SIZE>& out, const size_t block_idx)
/* @return number of bytes read on success, -1 on failure */
{
    out.fill('\0'); // 0 padding / clearing if not present

    if (block_idx + 1 > blk_counter) return -1; // if the idx not present
    
    FILE *file = fopen(file_path.c_str(), "rb");
    if (!file) { perror("File Handler: Can't open FileHandler file for read_block"); return -1; }

    fseek(file, RECORD_BYTES * block_idx, SEEK_SET);
    size_t read = fread(out.data(), 1, RECORD_BYTES, file);
    fclose(file);

    r_events++;
    return read;
}

size_t FileHandler::write_block(const std::array<uint8_t, DISK_PAGE_SIZE>& data, size_t block_idx, size_t n_bytes)
/* @return number of bytes written on success, -1 on failure */
{
    FILE *file = fopen(file_path.c_str(), "rb+");
    if (!file) { perror("File Handler: Can't open FileHandler file for write_block"); return -1; }

    if (block_idx + 1 > blk_counter)  block_idx = blk_counter++; // if idx not present, append at the end

    fseek(file, RECORD_BYTES * block_idx, SEEK_SET);
    size_t written = fwrite(data.data(), 1, n_bytes, file);
    fclose(file);

    w_events++;
    return written;
}

void FileHandler::reset() { blk_counter = 0; }

std::filesystem::path FileHandler::get_file_path() { return file_path; }
size_t FileHandler::get_blkcount() { return blk_counter; }
unsigned long long int FileHandler::get_reads() { return r_events; }
unsigned long long int FileHandler::get_writes() { return w_events; }


void FileHandler::print()
{
    FILE *checked_file = fopen(file_path.c_str(), "rb");
    if (!checked_file) { perror("sort: can't open file for cli_print"); return; }

    while(true)
    {
        double m, v;
        size_t read_m = fread(&m, sizeof(double), 1, checked_file);
        size_t read_v = fread(&v, sizeof(double), 1, checked_file);

        if (read_m != 1 || read_v != 1) break; // EOF

        long double val = Record::get_value(m, v);
        printf("%Lf\n", val);
    }

    fclose(checked_file);
}
