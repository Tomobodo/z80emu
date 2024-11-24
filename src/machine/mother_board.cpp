#include "mother_board.hpp"

MotherBoard::MotherBoard(unsigned int frequency) { set_frequency(frequency); };

void MotherBoard::reset() {
  m_control_bus = 0;
  m_last_time = std::chrono::high_resolution_clock::now();

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

void MotherBoard::set_frequency(unsigned long long frequency) {
  m_frequency = frequency;
  m_clock_delay = (1'000'000'000 / frequency) / 2;
  m_clock_time_acc = 0;
}

void MotherBoard::update() {
  auto now = std::chrono::high_resolution_clock::now();
  auto elapsed =
      std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_last_time);
  m_last_time = now;

  m_clock_time_acc += elapsed.count();

  while (m_clock_time_acc >= m_clock_delay) {
    clock(m_clock_phase = !m_clock_phase);

    m_clock_time_acc -= m_clock_delay;
  }

  double delta_time = static_cast<double>(elapsed.count()) / 1'000'000'000.0;
  for (auto *component : m_components) {
    component->update(delta_time);
  }

  m_last_time = now;
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
