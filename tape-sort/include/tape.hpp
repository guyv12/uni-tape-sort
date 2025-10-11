#pragma once

#ifndef tape_hpp
#define tape_hpp

#include <cstdio>
#include <cstdlib>
#include <queue>

#include "record.hpp"


class Tape
{
    public:
        
        void show();
        void show_at(long long int idx);


    private:

        long long int record_count;
        
        std::queue<Record> buffer;
        
};

#endif //tape_hpp
