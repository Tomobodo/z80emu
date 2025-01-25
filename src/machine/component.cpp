#include "component.hpp"

#include "control_bus.hpp"
#include "utils/bit_operations.hpp"

#include <cstdint>

bool Component::read_control_bus_pin(ControlBusPin pin) const {
  return get_bit_with_mask(m_control_bus_in, (uint16_t)pin);
}

void Component::write_control_bus_pin(ControlBusPin pin, bool value) {
  set_bit_with_mask(m_control_bus_out, (uint16_t)pin, value);
}
