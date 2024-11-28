#pragma once

#include "component.hpp"
#include "registers.hpp"

#include <cstdint>

class CPU : public Component {
public:
  CPU();
  ~CPU() {};

  void clock(bool clock_high) override;

  inline uint16_t get_program_counter() const {
    return get_register(Register_16::PC);
  }

  inline uint8_t get_register(Register_8 reg) const {
    return m_registers_memory[static_cast<int>(reg)];
  };

  inline uint16_t get_register(Register_16 reg) const {
    return *(reinterpret_cast<const uint16_t *>(m_registers_memory) +
             static_cast<int>(reg));
  };

private:
  void reset() override;

  inline void set_register(Register_8 reg, uint8_t value) {
    m_registers_memory[static_cast<int>(reg)] = value;
  }

  inline void set_register(Register_16 reg, uint16_t value) {
    *(reinterpret_cast<uint16_t *>(m_registers_memory) +
      static_cast<int>(reg)) = value;
  }

private:
  static constexpr uint8_t REGISTERS_BYTES_COUNT = 208 / 8;

  uint8_t m_m_cycle;
  uint8_t m_t_cycle;

  uint8_t m_registers_memory[REGISTERS_BYTES_COUNT];
};
