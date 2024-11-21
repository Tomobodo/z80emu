#include "mother_board.hpp"

MotherBoard::MotherBoard(unsigned int frequency) : m_frequency(frequency) {
  m_clock_delay = 1.0 / (double)frequency;
  m_clock_time_acc = 0;
};

void MotherBoard::reset() {
  m_control_bus = 0;

  for (auto *component : m_components) {
    component->reset();
  }
}

void MotherBoard::run() {
  reset();

  while (m_is_on) {
    update();
  }
}

void MotherBoard::update() {
  auto now = std::chrono::high_resolution_clock::now();
  auto duration =
      duration_cast<std::chrono::duration<double>>(now - m_last_time);
  double delta_time = duration.count();

  m_clock_time_acc += delta_time;

  while (m_clock_time_acc > m_clock_delay) {
    clock(true);
    clock(false);

    m_clock_time_acc -= m_clock_delay;
  }
}

void MotherBoard::clock(bool clock_high) {

  for (auto *component : m_components) {
    component->set_control_bus_in(m_control_bus);
    component->set_data_bus_in(m_data_bus);
    component->set_address_bus_in(m_address_bus);

    component->clock(clock_high);
  }

  m_control_bus = 0;
  m_data_bus = 0;
  m_address_bus = 0;

  for (auto *component : m_components) {
    m_control_bus |= component->get_control_bus_out();
    m_data_bus |= component->get_data_bus_out();
    m_address_bus |= component->get_address_bus_out();
  }
}
