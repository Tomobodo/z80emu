#pragma once

#include "component.hpp"

#include <cstdint>

class CPU : public Component {
public:
  CPU();
  ~CPU() {};

  void clock(bool clock_high) override;

private:
  void reset() override;

private:
  uint8_t m_m_cycle;
  uint8_t m_t_cycle;

  uint16_t m_program_counter = 0;
};
