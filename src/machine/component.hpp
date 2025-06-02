#pragma once

#include "control_bus.hpp"
#include "mother_board.hpp"
#include <cstdint>

struct ComponentConfig {
  ControlBus control_bus_in;
  ControlBus control_bus_out;
  uint16_t address_bus_in;
  uint16_t address_bus_out;
  uint8_t data_bus_in;
  uint8_t data_bus_out;
};

class Component {
public:
  Component() = default;
  Component(const Component &) = default;
  Component(Component &&) = delete;
  auto operator=(const Component &) -> Component & = default;
  auto operator=(Component &&) -> Component & = delete;

  Component(ComponentConfig config)
      : m_control_bus_in(config.control_bus_in),
        m_control_bus_out(config.control_bus_out),
        m_address_bus_in(config.address_bus_in),
        m_address_bus_out(config.address_bus_out),
        m_data_bus_in(config.data_bus_in), m_data_bus_out(config.data_bus_out) {
  }

  virtual ~Component() = default;

  virtual void update(double delta_time) {};

  virtual void clock(bool clock_high);

  virtual void reset() = 0;

  // control bus
  void set_control_bus_in(ControlBus bus_in) { m_control_bus_in = bus_in; };

  [[nodiscard]] auto get_control_bus_out() const -> ControlBus {
    return m_control_bus_out;
  }

  // data bus
  void set_data_bus_in(uint8_t bus_in);

  auto get_data_bus_in() -> uint8_t { return m_data_bus_in; }

  [[nodiscard]] auto get_data_bus_out() const -> uint8_t {
    return m_data_bus_out;
  }

  // address bus
  void set_address_bus_in(uint16_t bus_in) { m_address_bus_in = bus_in; }

  [[nodiscard]] auto get_address_bus_out() const -> uint16_t {
    return m_address_bus_out;
  }

protected:
  [[nodiscard]] auto control_bus_pin_changed_to(ControlBusPin pin,
                                                bool to_value) const -> bool;
  [[nodiscard]] auto read_control_bus_pin(ControlBusPin pin) const -> bool;
  void write_control_bus_pin(ControlBusPin pin, bool value);

  ControlBus m_previous_bus_in{};

  ControlBus m_control_bus_in{};
  ControlBus m_control_bus_out{};

  uint16_t m_address_bus_in{};
  uint16_t m_address_bus_out{};

  uint8_t m_data_bus_in{};
  uint8_t m_data_bus_out{};

private:
};
