#include "cpu.hpp"

CPU::CPU() { reset(); }

void CPU::reset() {
  m_m_cycle = 0;
  m_t_cycle = 0;

  // clear registers
  std::fill(m_registers_memory.data(), &*(m_registers_memory.end() - 1), 0);
}

void CPU::clock(bool clock_high) {
  uint16_t program_counter = get_register(Register_16::PC);

  if (read_control_bus_pin(ControlBusPin::RESET)) {
    reset();
    return;
  }

  if (read_control_bus_pin(ControlBusPin::WAIT)) {
    return;
  }

  switch (m_t_cycle) {
  case 0:
    write_control_bus_pin(ControlBusPin::MREQ, true);
    write_control_bus_pin(ControlBusPin::RD, true);
    write_control_bus_pin(ControlBusPin::M1, true);

    m_address_bus_out = program_counter;
    break;

  case 1:
    break;

  case 2: {
    uint8_t data = m_data_bus_in;

    write_control_bus_pin(ControlBusPin::MREQ, false);
    write_control_bus_pin(ControlBusPin::RD, false);
    write_control_bus_pin(ControlBusPin::M1, false);
  } break;

  case 3:
    set_register(Register_16::PC, program_counter + 1);
    break;
  }

  m_t_cycle++;
  if (m_t_cycle > 3) {
    m_t_cycle = 0;
  }
}
