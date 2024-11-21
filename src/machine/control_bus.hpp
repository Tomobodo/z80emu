#ifndef CONTROL_BUS_HPP
#define CONTROL_BUS_HPP

#include <cstdint>

using ControlBus = uint16_t;

enum class ControlBusPin : uint16_t {
  M1 = 1,
  MREQ = 2,
  IORQ = 4,
  RD = 8,
  WR = 16,
  RFSH = 32,
  HALT = 64,
  WAIT = 128,
  INT = 256,
  NMI = 512,
  RESET = 1024,
  BUSRQ = 2048,
  BUSACK = 4096
};

#endif
