#ifndef MEMORY_COMPONENT_HPP
#define MEMORY_COMPONENT_HPP

#include "machine_component.hpp"

#include <cstdint>

constexpr uint16_t MEMORY_SIZE = 0xFFFF;

class Memory : public MachineComponent {
public:
  Memory() {};

  void load_bytes(uint16_t address, const unsigned char *bytes,
                  uint16_t bytes_number);

  void reset() override;

  void clock() override;

private:
  unsigned char m_bytes[MEMORY_SIZE];
};

#endif
