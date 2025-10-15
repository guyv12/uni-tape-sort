#pragma once

#ifndef const_hpp
#define const_hpp

constexpr const int DISK_PAGE_SIZE = 512;                             // B in bytes
constexpr const int RECORD_SIZE = 2 * sizeof(double);                 // R in bytes
constexpr const int BLOCKING_FACTOR = DISK_PAGE_SIZE / RECORD_SIZE;   // b

constexpr const int BUFFER_COUNT = 10;                                // n 


#endif //const_hpp
