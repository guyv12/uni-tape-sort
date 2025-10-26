#pragma once

#ifndef record_hpp
#define record_hpp

#include <filesystem>


class Record
{
    public:

        Record();
        Record(double m, double v);

        int compare(const Record& other) const;

        long double get_value() const;
        static long double get_value(double m, double v);
        
        double m() const;
        double v() const;

    private:

        double mass;
        double velocity;
};


void generate_db(const std::filesystem::path& filename, long long int records);
void input_db(const std::filesystem::path& file_name);

#endif //record_hpp
