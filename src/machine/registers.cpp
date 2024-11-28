#include "registers.hpp"

#define X(name, value)                                                         \
  case Register_8::name:                                                       \
    return #name;
const char *register_8_to_name(Register_8 reg) {
  switch (reg) {
    REGISTER_8_LIST
  default:
    return "?";
  }
}
#undef X

#define X(name, value)                                                         \
  case Register_16::name:                                                      \
    return #name;
const char *register_16_to_name(Register_16 reg) {
  switch (reg) {
    REGISTER_16_LIST
  default:
    return "?";
  }
}
#undef X
