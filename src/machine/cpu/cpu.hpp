#pragma once

#include "../component.hpp"
#include "machine/cpu/instruction_executor.hpp"
#include "machine/cpu/z80_instruction_executor.hpp"
#include "operation.hpp"
#include "registers.hpp"
#include "utils/circular_queue.hpp"

#include <array>
#include <climits>
#include <cstdint>
#include <functional>

class CPU : public Component {
public:
  // ----- METHODS -----

  CPU();

  void clock(bool clock_active) override;

  [[nodiscard]] auto get_program_counter() const -> const uint16_t {
    return get_register(Register_16::PC);
  }

  [[nodiscard]] auto get_register(Register_8 reg) const -> const uint8_t {
    auto index = static_cast<uint8_t>(reg);
    return m_registers_memory.at(index);
  };

  [[nodiscard]] auto get_register(Register_16 reg) const -> const uint16_t {
    auto index = static_cast<uint8_t>(reg);
    return m_registers_memory.at(index) |
           (m_registers_memory.at(index + 1) << CHAR_BIT);
  };

  void set_register(Register_8 reg, uint8_t value) {
    auto index = static_cast<uint8_t>(reg);
    m_registers_memory.at(index) = value;
  }

  void set_register(Register_16 reg, uint16_t value) {
    auto index = static_cast<uint8_t>(reg);
    m_registers_memory.at(index) = value & UINT8_MAX;
    m_registers_memory.at(index + 1) = (value >> CHAR_BIT) & UINT8_MAX;
  }

  void push_operation(Operation operation);

  void set_halted(bool halted);

  [[nodiscard]] auto is_halted() const -> bool { return m_halted; }

  [[nodiscard]] auto is_waiting() const -> bool { return m_waiting; }

  void set_on_halted(std::function<void()> callback) {
    m_on_halted_callback = callback;
  }

private:
  // ----- METHODS -----

  void reset() override;

  void handle_operation(Operation &operation, bool clock_active);

  auto handle_opcode_fetch(bool clock_active) -> bool;

  auto handle_set_8_bit_register_direct(bool clock_active) -> bool;

  auto handle_memory_read(bool clock_active) -> bool;

  auto handle_memory_write(bool clock_active) -> bool;

  // ----- STATIC FIELDS -----
  static constexpr uint8_t REGISTERS_BYTES_COUNT = 208 / CHAR_BIT;
  static constexpr uint8_t OPERATION_QUEUE_SIZE = 8;
  static constexpr uint8_t REGISTERS_VALUE_ON_RESET = 0xFF;

  // ----- FIELDS -----
  bool m_waiting{};
  bool m_halted{};

  uint8_t m_time_cycle{};

  uint8_t m_sampled_data{};
  uint8_t m_interupt_mode{};
  uint8_t m_current_opcode{};

  bool m_IFF1{}, m_IFF2{};

  CircularQueue<Operation, OPERATION_QUEUE_SIZE> m_operation_queue;
  const Operation *m_current_operation = nullptr;

  std::array<uint8_t, REGISTERS_BYTES_COUNT> m_registers_memory{};

  Z80Executor m_instruction_executor;

  std::function<void()> m_on_halted_callback{[]() {}};
};
