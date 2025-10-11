#pragma once

#ifndef record_hpp
#define record_hpp

#include <string>
#include <iostream>

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


void generate_db(std::string filename, long long int records);


#endif //record_hpp
