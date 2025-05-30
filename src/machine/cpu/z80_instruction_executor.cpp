#include "z80_instruction_executor.hpp"

#include <cstdint>

#include "cpu.hpp"
#include "machine/cpu/registers.hpp"

constexpr uint8_t OP_TYPE_MASK = 0b11000000;
constexpr uint8_t OP_ARITHMETIC_MASK = 0b00111000;

enum class OpTypes : uint8_t {
  LOAD = 0b00000000,
  REGISTER_OP = 0b01000000,
  ARITHMETIC = 0b10000000,
  MISC_EX = 0b11000000
};

enum class OpArithmetic : uint8_t {
  ADD = 0b00000000,
  ADC = 0b00001000,
  SUB = 0b00010000,
  SBC = 0b00011000,
  AND = 0b00100000,
  XOR = 0b00101000,
  OR = 0b00110000,
  CP = 0b00111000
};

void Z80Executor::execute(uint8_t opcode, CPU *cpu) {

  if ((opcode & OP_TYPE_MASK) == (uint8_t)OpTypes::ARITHMETIC) {
    const uint8_t operation = opcode & OP_ARITHMETIC_MASK;

    switch (operation) {
    case (uint8_t)OpArithmetic::ADD: {
      const auto source_register = static_cast<Register_8>(opcode & 0b00000111);
      const uint8_t result =
          cpu->get_register(Register_8::A) + cpu->get_register(source_register);
      cpu->set_register(Register_8::A, result);
      return;
    } break;

    case (uint8_t)OpArithmetic::SUB: {
      const auto source_register = static_cast<Register_8>(opcode & 0b00000111);
      const uint8_t result =
          cpu->get_register(Register_8::A) - cpu->get_register(source_register);
      cpu->set_register(Register_8::A, result);
      return;
    } break;
    }
  }

  switch (opcode) {
  case 0x00: /* NOP */
    break;

  case 0x3E: /* LD A, n */
    cpu->push_operation(
        {.type = OperationType::SET_8_BIT_REGISTER_DIRECT,
         .source = static_cast<uint16_t>(cpu->get_program_counter() + 1),
         .dest = static_cast<uint16_t>(Register_8::A)});
    break;

  case 0x06: /* LD B, n */
    cpu->push_operation(
        {.type = OperationType::SET_8_BIT_REGISTER_DIRECT,
         .source = static_cast<uint16_t>(cpu->get_program_counter() + 1),
         .dest = static_cast<uint16_t>(Register_8::B)});
    break;

  case 0x0E: /* LD C, n */
    cpu->push_operation(
        {.type = OperationType::SET_8_BIT_REGISTER_DIRECT,
         .source = static_cast<uint16_t>(cpu->get_program_counter() + 1),
         .dest = static_cast<uint16_t>(Register_8::C)});
    break;

  case 0x16: /* LD D, n */
    cpu->push_operation(
        {.type = OperationType::SET_8_BIT_REGISTER_DIRECT,
         .source = static_cast<uint16_t>(cpu->get_program_counter() + 1),
         .dest = static_cast<uint16_t>(Register_8::D)});

    break;

  case 0x1E: /* LD E, n */
    cpu->push_operation(
        {.type = OperationType::SET_8_BIT_REGISTER_DIRECT,
         .source = static_cast<uint16_t>(cpu->get_program_counter() + 1),
         .dest = static_cast<uint16_t>(Register_8::E)});
    break;

  case 0x76: /* HALT */
    cpu->set_halted(true);
    break;
  }
}
