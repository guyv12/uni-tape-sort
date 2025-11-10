#include <cstdio>
#include "sort.hpp"


void test_quicksort();
void test_sort();


int main(void)
{
    test_quicksort();
    test_sort();

    return 0;
}


void test_quicksort()
{
    const char *const file_path = "qsort-file";
    generate_db(file_path, BLOCKING_FACTOR * BUFFER_COUNT);
    FileHandler file_handler(file_path);

    std::array<Buffer, BUFFER_COUNT> buffers;
    
    for (size_t i = 0; i < BUFFER_COUNT; i++)
        file_handler.read_block(buffers[i].array(), i);
            
    quick_sort(buffers, 0, (BUFFER_COUNT * BLOCKING_FACTOR) - 1);

    print_buffers(buffers);

    for(size_t i = 0; i < BUFFER_COUNT; i++)
        file_handler.write_block(buffers[i].array(), i);

    check_if_sorted(file_path);
    std::filesystem::remove(file_path);
}


void test_sort()
{
    const char *const file_path = "database-file";

    for (int i = 0; i < 11; i++)
    {
        size_t file_size = i * 100'000 + rand() % 5'000;
        printf("file size: %d", static_cast<int>(file_size));

        generate_db(file_path, file_size);
        sort(file_path);
        check_if_sorted(file_path);

        std::filesystem::remove(file_path);
    }
}