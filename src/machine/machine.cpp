#include <print>

#include "machine.hpp"

Machine::Machine(unsigned int frequency) : m_frequency(frequency) {
  m_clock_delay = 1.0 / (double)frequency;
  m_clock_time_acc = 0;

  m_components.push_back(reinterpret_cast<MachineComponent *>(&m_cpu));
  m_components.push_back(reinterpret_cast<MachineComponent *>(&m_memory));
};

void Machine::load_program(const unsigned char *program,
                           unsigned int program_size) {
  m_memory.load_bytes(0x0000, program, program_size);
}

void Machine::reset() {
  for (auto *component : m_components) {
    component->reset();
  }
}

void Machine::run() {
  reset();

  while (m_is_on) {
    update();
  }
}

void Machine::update() {
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

void Machine::clock() {
  for (auto *component : m_components) {
    component->clock();
  }
}
