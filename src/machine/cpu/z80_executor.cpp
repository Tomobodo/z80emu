#include "z80_executor.hpp"

#include <cstdint>

#include "cpu.hpp"
#include "machine/cpu/registers.hpp"

void Z80Executor::execute(uint8_t opcode, CPU *cpu) {
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

  case 0x80: /* ADD B */
  {
    const auto result =
        cpu->get_register(Register_8::A) + cpu->get_register(Register_8::B);
    cpu->set_register(Register_8::A, result);
  } break;
  }
}
