#include "cpu.hpp"
#include "machine/control_bus.hpp"
#include "registers.hpp"

#include <algorithm>
#include <cstdint>

CPU::CPU() { reset(); }

void CPU::reset() {
  m_time_cycle = 0;

  // clear registers
  std::ranges::fill(m_registers_memory, 0);
}

void CPU::clock(bool clock_active) {
  if (read_control_bus_pin(ControlBusPin::RESET)) {
    reset();
    return;
  }

  bool set_waiting =
      (m_time_cycle >= 1 && read_control_bus_pin(ControlBusPin::WAIT) &&
       !clock_active && !m_waiting);

  if (set_waiting || m_waiting) {
    m_waiting = !m_waiting;
    return;
  }

  if (m_operation_queue.empty()) {
    m_operation_queue.push({.type = OperationType::OPCODE_FETCH});
    m_current_operation = &m_operation_queue.head();
  }

  static constexpr auto operation_handlers =
      std::to_array<bool (CPU::*)(bool)>({&CPU::handle_opcode_fetch});

  const auto operation_index = static_cast<uint8_t>(m_current_operation->type);

  bool operation_finished =
      (this->*operation_handlers.at(operation_index))(clock_active);

  if (!clock_active) {
    if (operation_finished) {
      m_operation_queue.dequeue();
      m_time_cycle = 0;

      if (!m_operation_queue.empty()) {
        m_current_operation = &m_operation_queue.head();
      }
    } else {
      m_time_cycle++;
    }
  }
}

bool CPU::handle_opcode_fetch(bool clock_active) {
  uint16_t program_counter = get_register(Register_16::PC);

  switch (m_time_cycle) {
  case 0: {
    if (clock_active) {
      m_address_bus_out = program_counter;
      write_control_bus_pin(ControlBusPin::RFSH, false);
      write_control_bus_pin(ControlBusPin::M1, true);
    } else {
      write_control_bus_pin(ControlBusPin::MREQ, true);
      write_control_bus_pin(ControlBusPin::RD, true);

      set_register(Register_16::PC, program_counter + 1);
    }
  } break;

  case 1:
    // T2 is idle, waiting for memory to write on data bus
    break;

  case 2:
    write_control_bus_pin(ControlBusPin::M1, false);
    write_control_bus_pin(ControlBusPin::RFSH, true);

    if (clock_active) {
      m_instruction_register = m_data_bus_in;

      write_control_bus_pin(ControlBusPin::MREQ, false);
      write_control_bus_pin(ControlBusPin::RD, false);
    }

    break;

  default:
    return true;
  }

  return false;
}

bool CPU::handle_memory_read(bool clock_active) { return true; }

bool CPU::handle_memory_write(bool clock_active) { return true; }
