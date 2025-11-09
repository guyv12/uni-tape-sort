#include <cstdio>

#include "record.hpp"
#include "file-handler.hpp"
#include "sort.hpp"

void test_quicksort();


int main(void)
{
    test_quicksort();

    const char *const file_path = "database-file";
    generate_db(file_path, 1'000);
    sort(file_path);
    check_if_sorted(file_path);

    return 0;
}


void test_quicksort()
{
    generate_db("database-file", 100);
    FileHandler file_handler("database-file");

    std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT> buffers;
    
    for (size_t i = 0; i < BUFFER_COUNT; i++)
        file_handler.read_block(buffers[i], i);
            
    quick_sort(buffers, 0, (BUFFER_COUNT * BLOCKING_FACTOR) - 1);

    for (int i = 0; i < BUFFER_COUNT; i++)
    {
        double* arr = reinterpret_cast<double*>(buffers[i].data());

        for (int j = 0; j < BLOCKING_FACTOR; j++)
        {
            printf("%d: %0.2Lf ", i * BLOCKING_FACTOR + j, Record::get_value(arr[2 * j], arr[2 * j + 1]));
        }
        printf("\n");
    }

    for(size_t i = 0; i < BUFFER_COUNT; i++)
        file_handler.write_block(buffers[i], i);

    check_if_sorted("database-file");
}
