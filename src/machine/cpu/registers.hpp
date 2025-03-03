#pragma once

/// 16 bit registers are actually just two 8 bit registers.
/// They share the same memory space but I chose to
/// still represent them as 16 bit to ease debugging and value reading a bit

#include <cstdint>
#define REGISTER_8_LIST                                                        \
  X(A, 0)                                                                      \
  X(F, 1)                                                                      \
  X(B, 2)                                                                      \
  X(C, 3)                                                                      \
  X(D, 4)                                                                      \
  X(E, 5)                                                                      \
  X(H, 6)                                                                      \
  X(L, 7)                                                                      \
  X(I, 16)                                                                     \
  X(R, 17)                                                                     \
  X(A_P, 18)                                                                   \
  X(F_P, 19)                                                                   \
  X(B_P, 20)                                                                   \
  X(C_P, 21)                                                                   \
  X(D_P, 22)                                                                   \
  X(E_P, 23)                                                                   \
  X(H_P, 24)                                                                   \
  X(L_P, 25)

#define REGISTER_16_LIST                                                       \
  X(BC, 2)                                                                     \
  X(DE, 4)                                                                     \
  X(HL, 6)                                                                     \
  X(IX, 8)                                                                     \
  X(IY, 10)                                                                    \
  X(PC, 12)                                                                    \
  X(SP, 14)                                                                    \
  X(BC_P, 20)                                                                  \
  X(DE_P, 22)                                                                  \
  X(HL_P, 24)

constexpr unsigned int REGISTER_8_COUNT = 18;
constexpr unsigned int REGISTER_16_COUNT = 10;

#define X(name, value) name = value,
enum class Register_8 : uint8_t { REGISTER_8_LIST };

enum class Register_16 : uint8_t { REGISTER_16_LIST };
#undef X

const char *register_8_to_name(Register_8 reg);

const char *register_16_to_name(Register_16 reg);
