#ifndef MACHINE_HPP
#define MACHINE_HPP

#include <chrono>
#include <vector>

#include "cpu.hpp"
#include "memory.hpp"

class Machine {
public:
  Machine(unsigned int frequency);
  ~Machine() {};

  void load_program(const unsigned char *program, unsigned int program_size);

  void reset();

  void run();

private:
  void update();

  void clock();

private:
  bool m_is_on = true;

  std::chrono::high_resolution_clock::time_point m_last_time;
  double m_clock_time_acc;
  double m_clock_delay;

  unsigned int m_frequency;

  std::vector<MachineComponent *> m_components;

  CPU m_cpu;
  Memory m_memory;
};

#endif
