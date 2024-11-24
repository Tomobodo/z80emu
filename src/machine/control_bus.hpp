#pragma once

#include <cstdint>

using ControlBus = uint16_t;

#define CONTROL_BUS_PIN_LIST                                                   \
  X(M1, 1)                                                                     \
  X(MREQ, 2)                                                                   \
  X(IORQ, 4)                                                                   \
  X(RD, 8)                                                                     \
  X(WR, 16)                                                                    \
  X(RFSH, 32)                                                                  \
  X(HALT, 64)                                                                  \
  X(WAIT, 128)                                                                 \
  X(INT, 256)                                                                  \
  X(NMI, 512)                                                                  \
  X(RESET, 1024)                                                               \
  X(BUSRQ, 2048)                                                               \
  X(BUSACK, 4096)

#define X(name, value) name = value,
enum class ControlBusPin : uint16_t { CONTROL_BUS_PIN_LIST };
#undef X

#define X(name, value) #name,
constexpr const char *CONTROL_BUS_PIN_NAMES[] = {CONTROL_BUS_PIN_LIST};
#undef X

#define X(name, value) ControlBusPin::name,
constexpr const ControlBusPin CONTROL_BUS_PINS[] = {CONTROL_BUS_PIN_LIST};
#undef X
