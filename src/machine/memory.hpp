#pragma once

#include "component.hpp"

#include <array>
#include <cstdint>

constexpr uint16_t MEMORY_SIZE = 0xFFFF;

class Memory : public Component {
public:
  Memory() = default;

  uint8_t read_address(uint16_t address);

  uint8_t *get_address(uint16_t address);

  void load_bytes(uint16_t address, const uint8_t *bytes,
                  uint16_t bytes_number);

  void reset() override;

  void clock(bool clock_high) override;

private:
  std::array<uint8_t, MEMORY_SIZE> m_bytes{};
};
