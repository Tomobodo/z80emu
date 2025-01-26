#pragma once

#include "component.hpp"
#include "registers.hpp"

#include <climits>
#include <cstdint>

class CPU : public Component {
public:
  CPU();

  void clock(bool clock_high) override;

  [[nodiscard]] uint16_t get_program_counter() const {
    return get_register(Register_16::PC);
  }

  [[nodiscard]] uint8_t get_register(Register_8 reg) const {
    auto index = static_cast<uint8_t>(reg);
    return m_registers_memory.at(index);
  };

  [[nodiscard]] uint16_t get_register(Register_16 reg) const {
    auto index = static_cast<uint8_t>(reg);
    return m_registers_memory.at(index) << CHAR_BIT |
           m_registers_memory.at(index + 1);
  };

private:
  void reset() override;

  void set_register(Register_8 reg, uint8_t value) {
    auto index = static_cast<uint8_t>(reg);
    m_registers_memory.at(index) = value;
  }

  void set_register(Register_16 reg, uint16_t value) {
    auto index = static_cast<uint8_t>(reg);
    m_registers_memory.at(index) = value >> CHAR_BIT;
    m_registers_memory.at(index + 1) = value;
  }

  static constexpr uint8_t REGISTERS_BYTES_COUNT = 208 / CHAR_BIT;

  uint8_t m_m_cycle{};
  uint8_t m_t_cycle{};

  std::array<uint8_t, REGISTERS_BYTES_COUNT> m_registers_memory{};
};
