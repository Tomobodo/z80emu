#ifndef MEMORY_COMPONENT_HPP
#define MEMORY_COMPONENT_HPP

#include "mb_component.hpp"

#include <cstdint>

constexpr uint16_t MEMORY_SIZE = 0xFFFF;

class Memory : public MbComponent {
public:
  Memory() {};

  uint8_t read_address(uint16_t address);

  void load_bytes(uint16_t address, const uint8_t *bytes,
                  uint16_t bytes_number);

  void reset() override;

  void clock() override;

private:
  uint8_t m_bytes[MEMORY_SIZE];
};

#endif
