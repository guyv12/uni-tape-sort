#include "buffer.hpp"


void Buffer::set_size(size_t size) { _size = size; }

size_t Buffer::size() { return _size; }

std::array<uint8_t, DISK_PAGE_SIZE>& Buffer::array() { return _data; }
