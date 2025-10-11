#pragma once

#ifndef record_hpp
#define record_hpp

#include <filesystem>

class Record
{
    public:

        Record();

        long double get_value();
        double m();
        double v();

    private:

        double mass;
        double velocity;
};


void generate_db(const std::filesystem::path& filename, long long int records);


#endif //record_hpp
