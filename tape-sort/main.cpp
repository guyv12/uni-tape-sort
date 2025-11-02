#include <cstdio>

#include "record.hpp"
#include "file-handler.hpp"
#include "sort.hpp"


int main(void)
{
    generate_db("database-file", 1'000);
    FileHandler file_handler("database-file");

    std::array<std::array<uint8_t, DISK_PAGE_SIZE>, BUFFER_COUNT> buffers;
    
    for (int b = 0; b < BUFFER_COUNT; ++b) 
    {
        double* arr = reinterpret_cast<double*>(buffers[b].data());

        for (int i = 0; i < BLOCKING_FACTOR; i++) 
        {
            double m = static_cast<double>(b);       // e.g. buffer index
            double v = static_cast<double>(i);       // record index
            arr[i * 2]     = m;
            arr[i * 2 + 1] = v;
        }
    }
    
    quick_sort(buffers, 0, (BUFFER_COUNT * BLOCKING_FACTOR) - 1);

    for (int i = 0; i < BUFFER_COUNT; i++)
    {
        double* arr = reinterpret_cast<double*>(buffers[i].data());

        for (int j = 0; j < BLOCKING_FACTOR; j++)
        {
            printf("%0.2Lf ", Record::get_value(arr[2 * j], arr[2 * j + 1]));
        }
        printf("\n");
    }


    return 0;
}
