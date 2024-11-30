#include "memory.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>

void Memory::reset() {}

uint8_t Memory::read_address(uint16_t address) { return m_bytes[address]; }

uint8_t *Memory::get_address(uint16_t address) { return m_bytes + address; }

void Memory::load_bytes(uint16_t address, const uint8_t *bytes,
                        uint16_t bytes_number) {
  memcpy(m_bytes + address, bytes, bytes_number);
  std::fill(m_bytes + bytes_number, m_bytes + 0xFFFF - bytes_number, 0);
}

void Memory::clock(bool clock_high) {
  if (read_control_bus_pin(ControlBusPin::M1) &&
      read_control_bus_pin(ControlBusPin::MREQ) &&
      read_control_bus_pin(ControlBusPin::RD)) { // fetch op code op
    // eventually simulate delay here and a wait <F8>
    uint8_t data = m_bytes[m_address_bus_in];
    m_data_bus_out = data;
  }
}
