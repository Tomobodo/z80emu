#include "memory.hpp"
#include "machine/control_bus.hpp"

#include <cstdint>
#include <cstring>

void Memory::reset() {
  m_address_bus_out = 0;
  m_control_bus_out = 0;
}

auto Memory::read_address(uint16_t address) -> uint8_t {
  return m_bytes.at(address);
}

auto Memory::get_address(uint16_t address) -> uint8_t * {
  return &m_bytes.at(address);
}

void Memory::load_bytes(uint16_t address, const uint8_t *bytes,
			uint16_t bytes_number) {
  memcpy(&m_bytes.at(address), bytes, bytes_number);
}

void Memory::clock(bool clock_high) {
  if (read_control_bus_pin(ControlBusPin::RD) &&
      (control_bus_pin_changed_to(ControlBusPin::MREQ, true) ||
       m_memory_loading_clock_cycles > 0)) {

    // simulate memory delay here
    if (m_memory_loading_clock_cycles < READ_CLOCK_CYCLE_DELAY) {
      write_control_bus_pin(ControlBusPin::WAIT, true);
      m_memory_loading_clock_cycles++;
    } else {
      m_data_bus_out = m_bytes.at(m_address_bus_in);
      m_memory_loading_clock_cycles = 0;
      write_control_bus_pin(ControlBusPin::WAIT, false);
    }
  }

  Component::clock(clock_high);
}
