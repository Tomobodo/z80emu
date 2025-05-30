#pragma once

#include <cstdint>
class CPU;

class InstructionExecutor {
public:
  InstructionExecutor() = default;
  InstructionExecutor(const InstructionExecutor &) = default;
  InstructionExecutor(InstructionExecutor &&) = default;
  auto operator=(const InstructionExecutor &)
      -> InstructionExecutor & = default;
  auto operator=(InstructionExecutor &&) -> InstructionExecutor & = delete;
  virtual ~InstructionExecutor() = default;

  virtual void execute(uint8_t opcode, CPU *cpu) = 0;
};
