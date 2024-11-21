#ifndef CPU_HPP
#define CPU_HPP

#include "mb_component.hpp"

#include <cstdint>

enum class T_STATE { T1, T2, T3, T4 };

class CPU : MbComponent {
public:
  CPU();
  ~CPU() {};

  void clock() override;

private:
  void reset() override;

private:
  T_STATE m_t_state;

  uint16_t m_program_counter = 0;
};

#endif
