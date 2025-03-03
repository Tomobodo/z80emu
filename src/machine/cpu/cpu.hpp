#pragma once

#include "../component.hpp"
#include "operation.hpp"
#include "registers.hpp"
#include "utils/circular_queue.hpp"

#include <array>
#include <climits>
#include <cstdint>

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

  void handle_operation(Operation &operation, bool clock_active);

  bool handle_opcode_fetch(bool clock_active);

  bool handle_memory_read(bool clock_active);

  bool handle_memory_write(bool clock_active);

  // ----- STATIC FIELDS -----
  static constexpr uint8_t REGISTERS_BYTES_COUNT = 208 / CHAR_BIT;
  static constexpr uint8_t OPERATION_QUEUE_SIZE = 8;

  // ----- FIELDS -----
  bool m_waiting{};

  uint8_t m_time_cycle{};

  uint8_t m_sampled_data{};
  uint8_t m_instruction_register{};

  CircularQueue<Operation, OPERATION_QUEUE_SIZE> m_operation_queue;
  const Operation *m_current_operation;

  std::array<uint8_t, REGISTERS_BYTES_COUNT> m_registers_memory{};
};
