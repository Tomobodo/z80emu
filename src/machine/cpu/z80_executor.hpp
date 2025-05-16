#pragma once

#include "instruction_executor.hpp"
#include <cstdint>

class Z80Executor : public InstructionExecutor {
public:
  void execute(uint8_t opcode, CPU *cpu) override;
};
