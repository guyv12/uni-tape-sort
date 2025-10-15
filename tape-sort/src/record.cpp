#include "record.hpp"

#include <cmath>
#include <random>
#include <iostream>

Record::Record()
{
    static std::default_random_engine eg;
    static std::uniform_real_distribution<> dis(0, 100);

    mass = dis(eg);
    velocity = dis(eg);
}

Record::Record(double mass, double velocity) : mass(mass), velocity(velocity) { }


int Record::compare(const Record& other) const
{
    long double this_val = this->get_value(), other_val = other.get_value();

    if (this_val > other_val) return 1;
    if (this_val < other_val) return -1;

    return 0;
}

long double Record::get_value() const { return mass * pow(velocity, 2); }
double Record::m() const { return mass; }
double Record::v() const { return velocity; }


void generate_db(const std::filesystem::path& file_name, long long int records)
{
    FILE *db = fopen(file_name.c_str(), "wb");
    if (!db) perror("Record: Can't open db file"); return;

    for (long long int i = 0; i < records; i++)
    {
        Record r;

        double m, v;
        m = r.m(); v = r.v();

        fwrite(&m, sizeof(m), 1, db);
        fwrite(&v, sizeof(v), 1, db);
    }

    fclose(db);
}

void input_db(const std::filesystem::path& file_name)
{
    FILE *db = fopen(file_name.c_str(), "wb");
    if (!db) perror("Record: Can't open db file"); return;

    bool quit = false;
    while(!quit)
    {
        printf("Input record data in fp format (eg. 0.255)\n exit: q\n \n");

        std::string input;
        std::getline(std::cin, input);

        if (input == "q" || input == "Q") { quit = true; break; }

        double m, v;
        if (sscanf(input.c_str(), "%f %f", &m, &v) == 2)
        {
            fwrite(&m, sizeof(m), 1, db);
            fwrite(&v, sizeof(v), 1, db);
        }
        else
            printf("\nInvalid Input\n");
    }

    fclose(db);
    printf("database saved to %s", file_name.c_str());
}

