#pragma once

#define REGISTER_8_LIST                                                        \
  X(A, 0)                                                                      \
  X(F, 1)                                                                      \
  X(B, 2)                                                                      \
  X(C, 3)                                                                      \
  X(D, 4)                                                                      \
  X(E, 5)                                                                      \
  X(H, 6)                                                                      \
  X(L, 7)                                                                      \
  X(A_P, 8)                                                                    \
  X(F_P, 9)                                                                    \
  X(B_P, 10)                                                                   \
  X(C_P, 11)                                                                   \
  X(D_P, 12)                                                                   \
  X(E_P, 13)                                                                   \
  X(H_P, 14)                                                                   \
  X(L_P, 15)                                                                   \
  X(I, 16)                                                                     \
  X(R, 17)

#define REGISTER_16_LIST                                                       \
  X(BC, 1)                                                                     \
  X(DE, 2)                                                                     \
  X(HL, 3)                                                                     \
  X(BC_P, 5)                                                                   \
  X(DE_P, 6)                                                                   \
  X(HL_P, 7)                                                                   \
  X(IX, 9)                                                                     \
  X(IY, 10)                                                                    \
  X(SP, 11)                                                                    \
  X(PC, 12)

constexpr unsigned int REGISTER_8_COUNT = 18;
constexpr unsigned int REGISTER_16_COUNT = 10;

#define X(name, value) name = value,
enum class Register_8 : int { REGISTER_8_LIST };

enum class Register_16 : int { REGISTER_16_LIST };
#undef X

const char *register_8_to_name(Register_8 reg);

const char *register_16_to_name(Register_16 reg);
