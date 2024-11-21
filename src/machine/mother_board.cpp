#include <iostream>
#include <print>

#include "mother_board.hpp"

MotherBoard::MotherBoard(unsigned int frequency, const uint8_t *ROM,
                         unsigned int ROM_size)
    : m_frequency(frequency), m_ROM(ROM), m_ROM_size(ROM_size) {
  m_clock_delay = 1.0 / (double)frequency;
  m_clock_time_acc = 0;

  m_components.push_back(reinterpret_cast<MbComponent *>(&m_cpu));
  m_components.push_back(reinterpret_cast<MbComponent *>(&m_memory));
};

void MotherBoard::load_program(const uint8_t *program,
                               unsigned int program_size) {
  m_memory.load_bytes(0x0000, program, program_size);
}

void MotherBoard::reset() {
  m_control_bus = 0;

  for (auto *component : m_components) {
    component->reset();
  }
}

void MotherBoard::run(bool step_by_step) {
  reset();

  m_memory.load_bytes(0x0000, m_ROM, m_ROM_size);

  while (m_is_on) {
    if (!step_by_step) {
      update();
    } else { // step by step
      clock();

      std::println("Data bus: {:#04X}", m_data_bus);
      std::println("Address bus: {:#06X}", m_address_bus);
      std::println("Control bus: {}",
                   std::bitset<16>(m_control_bus).to_string());

      std::cin.ignore();
    }
  }
}

void MotherBoard::update() {
  auto now = std::chrono::high_resolution_clock::now();
  auto duration =
      duration_cast<std::chrono::duration<double>>(now - m_last_time);
  double delta_time = duration.count();

  m_clock_time_acc += delta_time;

  while (m_clock_time_acc > m_clock_delay) {
    clock();

    m_clock_time_acc -= m_clock_delay;
  }
}

void MotherBoard::clock() {

  for (auto *component : m_components) {
    component->set_control_bus_in(m_control_bus);
    component->set_data_bus_in(m_data_bus);
    component->set_address_bus_in(m_address_bus);

    component->clock();
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
