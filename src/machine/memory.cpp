#include "memory.hpp"

#include <cstdint>
#include <cstring>

void Memory::reset() {}

uint8_t Memory::read_address(uint16_t address) { return m_bytes.at(address); }

uint8_t *Memory::get_address(uint16_t address) { return &m_bytes.at(address); }

void Memory::load_bytes(uint16_t address, const uint8_t *bytes,
                        uint16_t bytes_number) {
  memcpy(&m_bytes.at(address), bytes, bytes_number);
}

void Memory::clock(bool clock_high) {
  if (read_control_bus_pin(ControlBusPin::M1) &&
      read_control_bus_pin(ControlBusPin::RD) &&
      control_bus_pin_changed_to(ControlBusPin::MREQ, true)) {

    // eventually simulate delay here and a wait
    m_data_bus_out = m_bytes.at(m_address_bus_in);
  }

  Component::clock(clock_high);
}
