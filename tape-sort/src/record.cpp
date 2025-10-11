#include "record.hpp"

#include <cmath>
#include <random>


Record::Record()
{
    static std::default_random_engine eg;
    static std::uniform_real_distribution<> dis(0, 100);

    mass = dis(eg);
    velocity = dis(eg);
}


long double Record::get_value() { return mass * pow(velocity, 2); }
double Record::m() { return mass; }
double Record::v() { return velocity; }


void generate_db(const std::filesystem::path& filename, long long int records)
{
    FILE *db = fopen(filename.c_str(), "wb");
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
