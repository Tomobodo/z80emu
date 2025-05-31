#include "z80_instruction_executor.hpp"

#include <cstdint>

#include "cpu.hpp"
#include "machine/cpu/registers.hpp"

constexpr uint8_t NOP = 0x00;
constexpr uint8_t HALT = 0x76;

constexpr uint8_t OP_TYPE_MASK = 0b11000000;

constexpr uint8_t OP_LOAD_ADDRESSING_MODE_MASK = 0b00000111;
constexpr uint8_t OP_LOAD_TARGET_MASK = 0b00111000;

constexpr uint8_t OP_ARITHMETIC_OPERATION_MASK = 0b00111000;

enum class OpTypes : uint8_t {
  LOAD = 0,        // 00 000 000
  REGISTER_OP = 1, // 01 000 000
  ARITHMETIC = 2,  // 10 000 000
  MISC_EX = 3      // 11 000 000
};

enum class OpLoadAddressingMode : uint8_t { IMMEDIATE = 6 };

enum class OpArithmetic : uint8_t {
  ADD = 0, // 00 000 000,
  ADC = 1, // 00 001 000,
  SUB = 2, // 00 010 000,
  SBC = 3, // 00 011 000,
  AND = 4, // 00 100 000,
  XOR = 5, // 00 101 000,
  OR = 6,  // 00 110 000,
  CP = 7,  // 00 111 000
};

void Z80Executor::execute(uint8_t opcode, CPU *cpu) {
  switch (opcode) {
  case NOP:
    break;

  case HALT:
    cpu->set_halted(true);
    break;
  default:
    break;
  }

  auto op_type = static_cast<OpTypes>(opcode & OP_TYPE_MASK);

  switch (op_type) {
  case OpTypes::LOAD:
    handle_load_ops(opcode, cpu);
    break;
  case OpTypes::ARITHMETIC:
    handle_arithmetic_ops(opcode, cpu);
    break;
  default:
    break;
  }
}

void Z80Executor::handle_arithmetic_ops(uint8_t opcode, CPU *cpu) {
  const uint8_t operation = opcode & OP_ARITHMETIC_OPERATION_MASK;

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

void Z80Executor::handle_load_ops(uint8_t opcode, CPU *cpu) {
  auto addressing_mode =
      static_cast<OpLoadAddressingMode>(opcode & OP_LOAD_ADDRESSING_MODE_MASK);

  if (addressing_mode == OpLoadAddressingMode::IMMEDIATE) {
    auto dest_register =
        static_cast<Register_8>((opcode & OP_LOAD_TARGET_MASK) >> 3);
    cpu->push_operation(
        {.type = OperationType::SET_8_BIT_REGISTER_IMMEDIATE,
         .source = static_cast<uint16_t>(cpu->get_program_counter() + 1),
         .dest = static_cast<uint16_t>(dest_register)});
  }
}
