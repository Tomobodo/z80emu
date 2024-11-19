#include "memory.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>

void Memory::reset() { std::fill(m_bytes, m_bytes + MEMORY_SIZE, 0); }

void Memory::load_bytes(uint16_t address, const unsigned char *bytes,
                        uint16_t bytes_number) {
  memcpy(m_bytes + address, bytes, bytes_number);
}

void Memory::clock() {}
