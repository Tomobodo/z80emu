#pragma once

#include <cstdint>

enum class OperationType : std::uint8_t {
  OPCODE_FETCH,
  SET_8_BIT_REGISTER_IMMEDIATE,
  MEMORY_RW,
  IO_RW,
  INTERUPT_ACK
};

struct Operation {
  OperationType type;
  uint16_t source;
  uint16_t dest;
};
