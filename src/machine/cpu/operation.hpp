#pragma once

#include <cstdint>

enum class OperationType : std::uint8_t {
  OPCODE_FETCH,
  MEMORY_READ,
  MEMORY_WRITE,
  IO_READ,
  IO_WRITE
};

struct Operation {
  OperationType type;
  uint16_t source;
  uint16_t dest;
};
