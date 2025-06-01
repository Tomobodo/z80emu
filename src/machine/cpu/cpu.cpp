#include "cpu.hpp"
#include "machine/control_bus.hpp"
#include "machine/cpu/operation.hpp"
#include "registers.hpp"

#include <algorithm>
#include <cstdint>

CPU::CPU() { reset(); }

void CPU::reset() {
  m_time_cycle = 0;
  m_waiting = false;
  set_halted(false);

  // z80-documented : Power on defaults
  std::ranges::fill(m_registers_memory, REGISTERS_VALUE_ON_RESET);

  set_register(Register_16::PC, 0);
  m_IFF1 = false;
  m_IFF2 = false;
  m_interupt_mode = 0;
  m_current_opcode = 0;

  m_current_operation = nullptr;
  m_operation_queue.clear();

  m_control_bus_out = 0;
  m_address_bus_out = 0;
}

void CPU::increment_pc() {
  set_register(Register_16::PC, get_register(Register_16::PC) + 1);
}

auto CPU::check_int() -> bool {
  bool bus_req = read_control_bus_pin(ControlBusPin::BUSRQ);
  return !bus_req && m_IFF1 && read_control_bus_pin(ControlBusPin::INT);
}

auto CPU::check_nmi() -> bool {
  bool bus_req = read_control_bus_pin(ControlBusPin::BUSRQ);
  return !bus_req && read_control_bus_pin(ControlBusPin::NMI);
}

void CPU::handle_nmi() {
  m_IFF1 = false;
  set_halted(false);
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

  bool operation_finished = false;

  switch (m_current_operation->type) {
  case OperationType::OPCODE_FETCH:
    operation_finished = handle_opcode_fetch(clock_active);
    break;
  case OperationType::MEMORY_READ:
    operation_finished = handle_memory_read(clock_active);
    break;
  default:
    break;
  }

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

void CPU::push_operation(Operation operation) {
  m_operation_queue.push(operation);
}

void CPU::set_halted(bool halted) {
  if (m_halted != halted && halted) {
    m_on_halted_callback();
  }
  m_halted = halted;
  write_control_bus_pin(ControlBusPin::HALT, halted);
}

auto CPU::handle_opcode_fetch(bool clock_active) -> bool {
  switch (m_time_cycle) {
  case 0: { // T1
    if (clock_active) {
      m_address_bus_out = get_register(Register_16::PC);
      write_control_bus_pin(ControlBusPin::RFSH, false);
      write_control_bus_pin(ControlBusPin::M1, true);
    } else {
      write_control_bus_pin(ControlBusPin::MREQ, true);
      write_control_bus_pin(ControlBusPin::RD, true);
    }
  } break;

  case 1: // T2
    // T2 is idle, waiting for memory to write on data bus
    break;

  case 2: // T3
    write_control_bus_pin(ControlBusPin::M1, false);
    write_control_bus_pin(ControlBusPin::RFSH, true);

    if (clock_active) {
      m_current_opcode = m_data_bus_in;

      write_control_bus_pin(ControlBusPin::MREQ, false);
      write_control_bus_pin(ControlBusPin::RD, false);

      if (!m_halted) {
        m_instruction_executor.execute(m_current_opcode, this);
      }
    }

    break;

  default: { // T4
    bool accept_nmi = m_halted && check_nmi();
    bool accept_int = m_halted && check_int();

    if (!clock_active) {
      if (!m_halted) {
        increment_pc();
      }

      return true;
    } else {
      if (accept_nmi) {
        handle_nmi();
      }
    }
  }
  }

  return false;
}

auto CPU::handle_memory_read(bool clock_active) -> bool {
  switch (m_time_cycle) {
  case 0: // T1
    if (clock_active) {
      m_address_bus_out = m_current_operation->source;
    } else {
      write_control_bus_pin(ControlBusPin::MREQ, true);
      write_control_bus_pin(ControlBusPin::RD, true);
    }
    break;

  case 1:  // T2
    break; // let memory write the value on the data bus

  case 2: { // T3
    if (clock_active) {
      auto target_register = static_cast<Register_8>(m_current_operation->dest);
      set_register(target_register, m_data_bus_in);
    } else {
      write_control_bus_pin(ControlBusPin::MREQ, false);
      write_control_bus_pin(ControlBusPin::RD, false);

      increment_pc();
      return true;
    }
  }
  }
  return false;
}

auto CPU::handle_memory_write(bool clock_active) -> bool { return true; }
