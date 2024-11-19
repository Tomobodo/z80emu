#include "cpu.hpp"

#include <cstring>

CPU::CPU() { reset(); }

void CPU::reset() { m_program_counter = 0; }

void CPU::set_data_bus(const unsigned char *value) {
  memcpy(m_data_bus, value, 8);
}

const unsigned char *CPU::get_data_bus() const { return m_data_bus; }

const unsigned char *CPU::get_address_bus() const { return m_address_bus; }

void CPU::clock() {
  if (m_reset_pin) {
    reset();
    return;
  }

  if (m_wait_pin) {
    return;
  }
}
