#ifndef MACHINE_HPP
#define MACHINE_HPP

#include <chrono>
#include <cstdint>
#include <vector>

#include "control_bus.hpp"
#include "cpu.hpp"
#include "memory.hpp"

class MotherBoard {
public:
  MotherBoard(unsigned int frequency, const uint8_t *ROM,
              unsigned int ROM_size);
  ~MotherBoard() {};

  void load_program(const uint8_t *program, unsigned int program_size);

  void reset();

  void run(bool step_by_step);

private:
  void update();

  void clock();

private:
  bool m_is_on = true;

  std::chrono::high_resolution_clock::time_point m_last_time;
  double m_clock_time_acc;
  double m_clock_delay;

  unsigned int m_frequency;

  std::vector<MbComponent *> m_components;

  ControlBus m_control_bus;
  uint16_t m_address_bus;
  uint8_t m_data_bus;

  CPU m_cpu;
  Memory m_memory;

  const uint8_t *m_ROM;
  uint16_t m_ROM_size;
};

#endif
