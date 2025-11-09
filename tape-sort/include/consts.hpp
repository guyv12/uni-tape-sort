#pragma once

#ifndef const_hpp
#define const_hpp

const int RECORD_FIELD_SIZE = sizeof(double);

constexpr const int DISK_PAGE_SIZE = 80;                             // B in bytes
constexpr const int RECORD_SIZE = 2 * RECORD_FIELD_SIZE;              // R in bytes
constexpr const int BLOCKING_FACTOR = DISK_PAGE_SIZE / RECORD_SIZE;   // b

constexpr const int RECORD_BYTES = BLOCKING_FACTOR * RECORD_SIZE;     // how many record bytes are in 1 disk page
constexpr const int PADDING_BYTES = DISK_PAGE_SIZE - RECORD_BYTES;    // how many bytes are used for padding in 1 disk page

constexpr const int BUFFER_COUNT = 3;                                // n 


#endif //const_hpp
