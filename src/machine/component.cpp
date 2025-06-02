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

void Component::clock(bool /*clock_active*/) {
  m_previous_bus_in = m_control_bus_in;
}

bool Component::control_bus_pin_changed_to(ControlBusPin pin,
                                           bool to_value) const {
  bool previous_pin_value = get_bit_with_mask(m_previous_bus_in, (uint16_t)pin);
  bool current_pin_value = get_bit_with_mask(m_control_bus_in, (uint16_t)pin);

  return current_pin_value == to_value &&
         previous_pin_value != current_pin_value;
}
void Component::set_data_bus_in(uint8_t bus_in) { m_data_bus_in = bus_in; }
