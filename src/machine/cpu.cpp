#include "cpu.hpp"

#include <cstring>

CPU::CPU() { reset(); }

void CPU::reset() {
  m_t_state = T_STATE::T1;
  m_program_counter = 0;
}

void CPU::clock() {
  if (read_control_bus_pin(ControlBusPin::RESET)) {
    reset();
    return;
  }

  if (read_control_bus_pin(ControlBusPin::WAIT)) {
    return;
  }

  switch (m_t_state) {
  case T_STATE::T1:
    write_control_bus_pin(ControlBusPin::MREQ, true);
    write_control_bus_pin(ControlBusPin::RD, true);
    write_control_bus_pin(ControlBusPin::M1, true);

    m_address_bus_out = m_program_counter;

    m_t_state = T_STATE::T2;
    break;

  case T_STATE::T2:
    m_t_state = T_STATE::T3;
    break;

  case T_STATE::T3: {
    uint8_t data = m_data_bus_in;

    write_control_bus_pin(ControlBusPin::MREQ, false);
    write_control_bus_pin(ControlBusPin::RD, false);
    write_control_bus_pin(ControlBusPin::M1, false);
    m_t_state = T_STATE::T4;
  } break;

  case T_STATE::T4:
    m_program_counter++;
    m_t_state = T_STATE::T1;
    break;
  }
}
