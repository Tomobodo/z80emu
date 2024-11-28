#pragma once

#include <chrono>
#include <cstdint>
#include <vector>

#include "control_bus.hpp"

class Component;

class MotherBoard {
public:
  MotherBoard(unsigned int frequency);
  ~MotherBoard() {};

  template <typename T>
  typename std::enable_if<std::is_base_of<Component, T>::value>::type
  add_component(T &component) {
    m_components.push_back(&component);
    component.set_mother_board(this);
  }

  void reset();

  void run();

  void set_clock_frequency(unsigned long long frequency);

  void power_off();

  inline void set_clock_paused(bool clock_paused) {
    m_clock_paused = clock_paused;
  }

  inline bool get_emulation_lagging() { return m_emulation_lagging; };

  inline unsigned long long get_frequency() { return m_frequency; };

private:
  void update();

  void clock(bool clock_high);

private:
  static constexpr unsigned int MAX_CATCHUP_CYCLES_NUM = 250'000;

  bool m_is_on = true;
  bool m_clock_phase = false;
  bool m_clock_paused = false;
  bool m_emulation_lagging = false;

  std::chrono::high_resolution_clock::time_point m_last_time;
  unsigned long long m_clock_time_acc;
  unsigned long long m_clock_delay;

  unsigned long long m_frequency;

  std::vector<Component *> m_components;

  ControlBus m_control_bus;
  uint16_t m_address_bus;
  uint8_t m_data_bus;
};
