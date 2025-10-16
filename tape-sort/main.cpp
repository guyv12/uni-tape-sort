#include <cstdio>

#include "record.hpp"
#include "file-handler.hpp"
#include "sort.hpp"


int main(void)
{
    generate_db("database-file", 1'000);

    FileHandler file_handler("tape-file");

    std::array<uint8_t, DISK_PAGE_SIZE> arr;
    
    arr.fill('1');
    file_handler.write_block(arr, 0);

    arr.fill('2');
    file_handler.write_block(arr, 1);

    arr.fill('3');
    file_handler.write_block(arr, 1);

    printf("%lld, %d\n", file_handler.get_writes(), file_handler.get_blkcount());
    
    std::array<uint8_t, DISK_PAGE_SIZE> out;
    file_handler.read_block(out, 1);

    printf("%lld\n", file_handler.get_reads());
    for (int i = 0; i < 512; i++)
        printf("%c", out[i]);

    return 0;
}
