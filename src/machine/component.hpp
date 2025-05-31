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
  Component &operator=(const Component &) = default;
  Component &operator=(Component &&) = delete;

  Component(ComponentConfig config, MotherBoard *mother_board_ptr)
      : m_control_bus_in(config.control_bus_in),
        m_control_bus_out(config.control_bus_out),
        m_address_bus_in(config.address_bus_in),
        m_address_bus_out(config.address_bus_out),
        m_data_bus_in(config.data_bus_in), m_data_bus_out(config.data_bus_out),
        m_mother_board(mother_board_ptr) {}

  virtual ~Component() = default;

  virtual void update(double delta_time) {};

  virtual void clock(bool clock_high);

  virtual void reset() = 0;

  // control bus
  void set_control_bus_in(ControlBus bus_in) { m_control_bus_in = bus_in; };

  [[nodiscard]] ControlBus get_control_bus_out() const {
    return m_control_bus_out;
  }

  // data bus
  void set_data_bus_in(uint8_t bus_in) { m_data_bus_in = bus_in; }

  [[nodiscard]] uint8_t get_data_bus_out() const { return m_data_bus_out; }

  // address bus
  void set_address_bus_in(uint16_t bus_in) { m_address_bus_in = bus_in; }

  [[nodiscard]] uint16_t get_address_bus_out() const {
    return m_address_bus_out;
  }

  void set_mother_board(MotherBoard *mother_board) {
    m_mother_board = mother_board;
    on_added();
  }

protected:
  [[nodiscard]] auto control_bus_pin_changed_to(ControlBusPin pin,
                                                bool to_value) const -> bool;
  [[nodiscard]] auto read_control_bus_pin(ControlBusPin pin) const -> bool;
  void write_control_bus_pin(ControlBusPin pin, bool value);

  virtual void on_added() {};

  ControlBus m_previous_bus_in{};

  ControlBus m_control_bus_in{};
  ControlBus m_control_bus_out{};

  uint16_t m_address_bus_in{};
  uint16_t m_address_bus_out{};

  uint8_t m_data_bus_in{};
  uint8_t m_data_bus_out{};

  MotherBoard *m_mother_board{};

private:
};
