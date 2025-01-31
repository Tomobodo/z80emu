#include "cpu.hpp"
#include "machine/control_bus.hpp"

#include <algorithm>

CPU::CPU() { reset(); }

void CPU::reset() {
  m_machine_cycle = MachineCycle::OPCODE_FETCH;
  m_t_cycle = 0;

  // clear registers
  std::ranges::fill(m_registers_memory, 0);
}

void CPU::clock(bool clock_active) {
  if (read_control_bus_pin(ControlBusPin::RESET)) {
    reset();
    return;
  }

  bool set_waiting =
      (m_t_cycle >= 1 && read_control_bus_pin(ControlBusPin::WAIT) &&
       !clock_active && !m_waiting);

  if (set_waiting || m_waiting) {
    m_waiting = !m_waiting;
    return;
  }

  bool change_machine_cycle = false;

  switch (m_machine_cycle) {
  case MachineCycle::OPCODE_FETCH:
    change_machine_cycle = handle_opcode_fetch(clock_active);
    break;
  case MachineCycle::MEMORY_READ:
    change_machine_cycle = handle_memory_read(clock_active);
    break;
  case MachineCycle::MEMORY_WRITE:
    change_machine_cycle = handle_memory_write(clock_active);
    break;
  }

  if (!clock_active) {
    if (change_machine_cycle) {
      m_t_cycle = 0;
      m_machine_cycle = static_cast<MachineCycle>(
          (static_cast<int>(m_machine_cycle) + 1) % 3);
    } else {
      m_t_cycle++;
    }
  }
}

bool CPU::handle_opcode_fetch(bool clock_active) {
  uint16_t program_counter = get_register(Register_16::PC);

  switch (m_t_cycle) {
  case 0: {
    if (clock_active) {
      m_address_bus_out = program_counter;
      write_control_bus_pin(ControlBusPin::M1, true);
    } else {
      write_control_bus_pin(ControlBusPin::MREQ, true);
      write_control_bus_pin(ControlBusPin::RD, true);
    }
  } break;

  case 1:
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

  case 3:

  default:
    if (!clock_active) {
      write_control_bus_pin(ControlBusPin::RFSH, false);
      set_register(Register_16::PC, program_counter + 1);
      return true;
    }
  }

  return false;
}

bool CPU::handle_memory_read(bool clock_active) { return true; }

bool CPU::handle_memory_write(bool clock_active) { return true; }
