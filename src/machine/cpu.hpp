#ifndef CPU_HPP
#define CPU_HPP

#include "mb_component.hpp"

#include <cstdint>

class CPU : MbComponent {
public:
  CPU();
  ~CPU() {};

  void clock() override;

private:
  void reset() override;

private:
  uint8_t m_m_cycle;
  uint8_t m_t_cycle;

  uint16_t m_program_counter = 0;
};

#endif
