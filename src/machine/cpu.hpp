#ifndef CPU_HPP
#define CPU_HPP

#include "machine_component.hpp"

#include <cstdint>

class CPU : MachineComponent {
public:
  CPU();
  ~CPU() {};

  inline void set_reset(bool value) { m_reset_pin = value; };
  inline void set_wait(bool value) { m_wait_pin = value; };

  inline bool get_halt() const { return m_halt_pin; };

  inline bool get_rd_pin() const { return m_rd_pin; };
  inline bool get_mreq_pin() const { return m_mreq_pin; };

  void set_data_bus(const unsigned char *value);
  const unsigned char *get_data_bus() const;

  const unsigned char *get_address_bus() const;

  void clock() override;

private:
  void reset() override;

private:
  bool m_reset_pin = false;
  bool m_wait_pin = false;
  bool m_halt_pin = false;
  bool m_mreq_pin = false;
  bool m_iorq_pin = false;
  bool m_rd_pin = false;
  bool m_wr_pin = false;
  bool m_m1_pin = false;

  unsigned char m_data_bus[8];
  unsigned char m_address_bus[16];

  uint16_t m_program_counter = 0;
};

#endif
