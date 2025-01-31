#include "mother_board.hpp"

#include "component.hpp"
#include "utils/constants.hpp"

MotherBoard::MotherBoard(const unsigned int frequency) {
  set_clock_frequency(frequency);
};

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

void MotherBoard::set_clock_frequency(const unsigned long long frequency) {
  m_frequency = frequency;
  m_clock_delay = (NANOSECONDS_IN_ONE_SECOND / frequency) / 2;
  m_clock_time_acc = 0;
}

void MotherBoard::power_off() { m_is_on = false; }

void MotherBoard::update() {
  const auto now = std::chrono::high_resolution_clock::now();

  const auto elapsed =
      std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_last_time);
  m_last_time = now;

  if (!m_clock_paused) {
    m_clock_time_acc += elapsed.count();

    auto cycles_to_do = m_clock_time_acc / m_clock_delay;

    if (cycles_to_do > MAX_CATCHUP_CYCLES_NUM) {
      m_emulation_lagging = true;
      cycles_to_do = MAX_CATCHUP_CYCLES_NUM;
    } else {
      m_emulation_lagging = false;
    }

    for (auto i = 0; i < cycles_to_do; i++) {
      clock(m_clock_phase = !m_clock_phase);
    }

    m_clock_time_acc -= cycles_to_do * m_clock_delay;
  }

  const double delta_time =
      static_cast<double>(elapsed.count()) / NANOSECONDS_IN_ONE_SECOND;
  for (auto *component : m_components) {
    component->update(delta_time);
  }

  m_last_time = now;
}

void MotherBoard::clock(const bool clock_active) {
  for (auto *component : m_components) {
    component->set_control_bus_in(m_control_bus);
    component->set_data_bus_in(m_data_bus);
    component->set_address_bus_in(m_address_bus);

    component->clock(clock_active);
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
