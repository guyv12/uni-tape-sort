#pragma once

#ifndef tape_hpp
#define tape_hpp

#include <cstdio>
#include <cstdlib>
#include <queue>

#include "record.hpp"
#include "file-handler.hpp"


class Tape
{
    public:
        
        void show();
        void show_at(long long int idx);


    private:

        FileHandler file_handler;
};


#endif //tape_hpp
