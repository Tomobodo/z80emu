#pragma once

#include "control_bus.hpp"
#include <cstdint>

class Component {
public:
  Component() {}
  virtual ~Component() {}

  virtual void clock(bool clock_high) = 0;

  virtual void reset() = 0;

  // control bus
  inline void set_control_bus_in(ControlBus bus_in) {
    m_control_bus_in = bus_in;
  };

  inline ControlBus get_control_bus_out() { return m_control_bus_out; }

  // data bus
  inline void set_data_bus_in(uint8_t bus_in) { m_data_bus_in = bus_in; }

  inline uint8_t get_data_bus_out() { return m_data_bus_out; }

  // address bus
  inline void set_address_bus_in(uint16_t bus_in) { m_address_bus_in = bus_in; }

  inline uint16_t get_address_bus_out() { return m_address_bus_out; }

protected:
  bool read_control_bus_pin(ControlBusPin pin);
  void write_control_bus_pin(ControlBusPin pin, bool value);

protected:
  ControlBus m_control_bus_in;
  ControlBus m_control_bus_out;

  uint16_t m_address_bus_in;
  uint16_t m_address_bus_out;

  uint8_t m_data_bus_in;
  uint8_t m_data_bus_out;

private:
};
