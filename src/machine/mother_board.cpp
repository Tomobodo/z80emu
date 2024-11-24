#include "mother_board.hpp"

#include "component.hpp"

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

void MotherBoard::power_off() { m_is_on = false; }

void MotherBoard::update() {
  auto now = std::chrono::high_resolution_clock::now();
  auto elapsed =
      std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_last_time);
  m_last_time = now;

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
