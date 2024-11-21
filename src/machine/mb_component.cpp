#include "mb_component.hpp"

#include "control_bus.hpp"
#include <cstdint>

bool MbComponent::read_control_bus_pin(ControlBusPin pin) {
  return (m_control_bus_in & static_cast<uint16_t>(pin)) > 0;
}

void MbComponent::write_control_bus_pin(ControlBusPin pin, bool value) {
  if (value)
    m_control_bus_out |= static_cast<uint16_t>(pin);
  else
    m_control_bus_out &= ~static_cast<uint16_t>(pin);
}
