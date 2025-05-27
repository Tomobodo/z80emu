#pragma once

#include "component.hpp"

#include <array>
#include <cstdint>

constexpr uint32_t MEMORY_SIZE = 0x10000;

class Memory : public Component {
public:
  Memory() = default;

  auto read_address(uint16_t address) -> uint8_t;

  void load_bytes(uint16_t address, const uint8_t *bytes,
                  uint16_t bytes_number);

  void reset() override;

  void clock(bool clock_high) override;

private:
  static constexpr size_t READ_CLOCK_CYCLE_DELAY = 0;
  std::array<uint8_t, MEMORY_SIZE> m_bytes{};

  int m_memory_loading_clock_cycles = 0;
};
