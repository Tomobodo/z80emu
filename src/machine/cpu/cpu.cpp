#include "cpu.hpp"
#include "machine/control_bus.hpp"
#include "registers.hpp"

#include <algorithm>
#include <cstdint>
#include <exception>
#include <format>
#include <iostream>

CPU::CPU() {
  m_operation_handlers.resize(30);
  m_operation_handlers[(size_t)OperationType::OPCODE_FETCH] =
      &CPU::handle_opcode_fetch;
  m_operation_handlers[(size_t)OperationType::SET_8_BIT_REGISTER_DIRECT] =
      &CPU::handle_set_8_bit_register_direct;
  m_operation_handlers[(size_t)OperationType::ALU_ADD] = &CPU::handle_alu_add;
  reset();
}

void CPU::reset() {
  m_time_cycle = 0;

  // z80-documented : Power on defaults
  std::ranges::fill(m_registers_memory, REGISTERS_VALUE_ON_RESET);

  set_register(Register_16::PC, 0);
  m_IFF1 = false;
  m_IFF2 = false;
  m_interupt_mode = 0;
  m_current_opcode = 0;

  m_current_operation = nullptr;
  m_operation_queue.clear();
}

void CPU::clock(bool clock_active) {
  if (read_control_bus_pin(ControlBusPin::RESET) && !clock_active) {
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
    push_operation({.type = OperationType::OPCODE_FETCH});
    m_current_operation = &m_operation_queue.head();
  }

  const auto operation_index = static_cast<uint8_t>(m_current_operation->type);

  try {
    bool operation_finished =
        (this->*m_operation_handlers.at(operation_index))(clock_active);

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
  } catch (std::exception &e) {
    std::print(std::cerr, "Error executing operation : {}\n", e.what());
  }
}

void CPU::push_operation(Operation operation) {
  m_operation_queue.push(operation);
}

auto CPU::handle_opcode_fetch(bool clock_active) -> bool {
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
    }
  } break;

  case 1:
    // T2 is idle, waiting for memory to write on data bus
    break;

  case 2:
    write_control_bus_pin(ControlBusPin::M1, false);
    write_control_bus_pin(ControlBusPin::RFSH, true);

    if (clock_active) {
      m_current_opcode = m_data_bus_in;

      write_control_bus_pin(ControlBusPin::MREQ, false);
      write_control_bus_pin(ControlBusPin::RD, false);

      if (m_instruction_executor.get()) {
        m_instruction_executor->execute(m_current_opcode, this);
      }
    }

    break;

  default:
    if (!clock_active) {
      set_register(Register_16::PC, program_counter + 1);

      return true;
    }
  }

  return false;
}

auto CPU::handle_set_8_bit_register_direct(bool clock_active) -> bool {
  uint16_t program_counter = get_register(Register_16::PC);

  switch (m_time_cycle) {
  case 0:
    if (clock_active) {
      m_address_bus_out = m_current_operation->source;
      write_control_bus_pin(ControlBusPin::RFSH, false);
      write_control_bus_pin(ControlBusPin::M1, true);
    } else {
      write_control_bus_pin(ControlBusPin::MREQ, true);
      write_control_bus_pin(ControlBusPin::RD, true);
    }
    break;

  case 1:
    break; // let memory write the value on the data bus

  case 2: {
    write_control_bus_pin(ControlBusPin::M1, false);
    write_control_bus_pin(ControlBusPin::RFSH, true);

    if (clock_active) {
      write_control_bus_pin(ControlBusPin::MREQ, false);
      write_control_bus_pin(ControlBusPin::RD, false);

      auto target_register = static_cast<Register_8>(m_current_operation->dest);
      set_register(target_register, m_data_bus_in);
    }

  } break;

  default:
    if (!clock_active) {
      set_register(Register_16::PC, program_counter + 1);
      return true;
    }
  }

  return false;
};

auto CPU::handle_memory_read(bool clock_active) -> bool { return true; }

auto CPU::handle_memory_write(bool clock_active) -> bool { return true; }

auto CPU::handle_alu_add(bool clock_active) -> bool {
  if (!clock_active) {
    auto source_register = static_cast<Register_8>(m_current_operation->source);
    auto dest_register = static_cast<Register_8>(m_current_operation->dest);
    auto dest_value = get_register(dest_register);
    auto source_value = get_register(source_register);

    auto result = dest_value + source_value;
    set_register(dest_register, result);

    set_register(Register_16::PC, get_register(Register_16::PC) + 1);
    return true;
  }

  return false;
};
