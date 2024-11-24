#pragma once

#include <chrono>
#include <cstdint>
#include <vector>

#include "component.hpp"
#include "control_bus.hpp"

class MotherBoard {
public:
  MotherBoard(unsigned int frequency);
  ~MotherBoard() {};

  template <typename T>
  typename std::enable_if<std::is_base_of<Component, T>::value>::type
  add_component(T &component) {
    m_components.push_back(&component);
  }

  void reset();

  void run();

  void set_frequency(unsigned long long frequency);

private:
  void update();

  void clock(bool clock_high);

private:
  bool m_is_on = true;
  bool m_clock_phase = false;

  std::chrono::high_resolution_clock::time_point m_last_time;
  unsigned long long m_clock_time_acc;
  unsigned long long m_clock_delay;

  unsigned long long m_frequency;

  std::vector<Component *> m_components;

  ControlBus m_control_bus;
  uint16_t m_address_bus;
  uint8_t m_data_bus;
};
