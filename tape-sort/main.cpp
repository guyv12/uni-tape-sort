#include <cstdio>

#include "tape.hpp"


int main(void)
{
    generate_db("database-file", 1'000);

    FileHandler file_handler("tape-file");
    
    for (int i = 0; i < 512; i++)
        file_handler.write('1');
    for (int i = 0; i < 512; i++)
        file_handler.write('2');

    printf("%lld, %d\n", file_handler.get_writes(), file_handler.get_blkcount());
    
    std::array<char, BLOCK_SIZE> out;
    file_handler.read(out, 1);

    printf("%lld\n", file_handler.get_reads());
    for (int i = 0; i < 512; i++)
        printf("%c", out[i]);

    return 0;
}
