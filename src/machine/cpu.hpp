#pragma once

#include "component.hpp"
#include "registers.hpp"

#include <array>
#include <climits>
#include <cstdint>

enum class MachineCycle : uint8_t {
  OPCODE_FETCH = 0,
  MEMORY_READ = 1,
  MEMORY_WRITE = 2
};

class CPU : public Component {
public:
  // ----- METHODS -----

  CPU();

  void clock(bool clock_active) override;

  [[nodiscard]] auto get_program_counter() const -> const uint16_t & {
    return get_register(Register_16::PC);
  }

  [[nodiscard]] const uint8_t &get_register(Register_8 reg) const {
    auto index = static_cast<uint8_t>(reg);
    return m_registers_memory.at(index);
  };

  [[nodiscard]] const uint16_t &get_register(Register_16 reg) const {
    auto index = static_cast<uint8_t>(reg);
    return reinterpret_cast<const uint16_t &>(m_registers_memory.at(index));
  };

private:
  // ----- METHODS -----

  void reset() override;

  void set_register(Register_8 reg, uint8_t value) {
    auto index = static_cast<uint8_t>(reg);
    m_registers_memory.at(index) = value;
  }

  void set_register(Register_16 reg, uint16_t value) {
    auto index = static_cast<uint8_t>(reg);
    reinterpret_cast<uint16_t &>(m_registers_memory.at(index)) = value;
  }

  bool handle_opcode_fetch(bool clock_active);

  bool handle_memory_read(bool clock_active);

  bool handle_memory_write(bool clock_active);

  // ----- STATIC FIELDS -----

  static constexpr uint8_t REGISTERS_BYTES_COUNT = 208 / CHAR_BIT;

  // ----- FIELDS -----
  bool m_waiting{};

  uint8_t m_t_cycle{};
  MachineCycle m_machine_cycle{};

  uint8_t m_sampled_data{};
  uint8_t m_instruction_register{};

  std::array<uint8_t, REGISTERS_BYTES_COUNT> m_registers_memory{};
};
