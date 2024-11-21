#include "component.hpp"

#include "control_bus.hpp"
#include <cstdint>

bool Component::read_control_bus_pin(ControlBusPin pin) {
  return (m_control_bus_in & static_cast<uint16_t>(pin)) > 0;
}

void Component::write_control_bus_pin(ControlBusPin pin, bool value) {
  if (value)
    m_control_bus_out |= static_cast<uint16_t>(pin);
  else
    m_control_bus_out &= ~static_cast<uint16_t>(pin);
}
