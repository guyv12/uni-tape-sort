#include "cli.hpp"

#include <iostream>
#include "sort.hpp"

void c_launch_sort();
void c_launch_sort_v();
void c_gen_db();
void c_in_db();
void c_print();

void cli()
{
    bool running = true;

    while (running)
    {
        std::cout << "----- Tape sort with large buffers -----\n\n"
        << "Avaliable options:\n\n"
        << "1) Launch Sort\n"
        << "2) Launch Sort verbose\n"
        << "3) Generate DB file\n"
        << "4) Input DB file\n"
        << "5) Print file\n"
        << "6) Exit\n\n"
        << "Choose an option: ";

        int input = 0;
        std::cin >> input;

        switch (input)
        {
            case 1: c_launch_sort(); break;
            case 2: c_launch_sort_v(); break;
            case 3: c_gen_db(); break;
            case 4: c_in_db(); break;
            case 5: c_print(); break;
            case 6: running = false; break;

            default:
                std::cout << "Invalid option. Try again.\n";
                break;
        }
    }
}


void c_launch_sort()
{
    std::cout << "\nInput db file name: ";
    std::string file_name;
    std::cin >> file_name;

    sort(file_name);
}

void c_launch_sort_v()
{
    std::cout << "\nInput db file name: ";
    std::string file_name;
    std::cin >> file_name;

    sort(file_name, true);
}

void c_gen_db()
{
    std::cout << "\nInput db file name: ";
    std::string file_name;
    std::cin >> file_name;

    std::cout << "\nInput db file size (number of records): ";
    int file_size;
    std::cin >> file_size;

    generate_db(file_name, file_size);
}

void c_in_db()
{
    std::cout << "\nInput db file name: ";
    std::string file_name;
    std::cin >> file_name;

    input_db(file_name);
}

void c_print()
{
    std::cout << "\nInput db file name: ";
    std::string file_name;
    std::cin >> file_name;

    FILE *checked_file = fopen(file_name.c_str(), "rb");
    if (!checked_file) { perror("sort: can't open file for cli_print"); return; }

    while(true)
    {
        double m, v;
        size_t read_m = fread(&m, sizeof(double), 1, checked_file);
        size_t read_v = fread(&v, sizeof(double), 1, checked_file);

        if (read_m != 1 || read_v != 1) break; // EOF

        long double val = Record::get_value(m, v);
        std::cout << val << "\n";
    }

    fclose(checked_file);
}