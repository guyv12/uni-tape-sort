#pragma once

#ifndef buffer_hpp
#define buffer_hpp

#include "consts.hpp"
#include <cstdlib>
#include <cstdint>
#include <array>


class Buffer
{
    public:

        void set_size(size_t size);

        size_t size();
        std::array<uint8_t, DISK_PAGE_SIZE>& array();

    private:

        std::array<uint8_t, DISK_PAGE_SIZE> _data;
        size_t _size;
};


#endif //buffer_hpp