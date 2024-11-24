#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>

#include "debug/debug_card.hpp"

#include "machine/cpu.hpp"
#include "machine/memory.hpp"
#include "machine/mother_board.hpp"

namespace fs = std::filesystem;

constexpr uint8_t REGISTERS_COUNT = 0b111;
constexpr uint16_t RAM_SIZE = 0xFFFF;

constexpr unsigned int FREQ_4MHZ = 4'000'000;

enum class Register : uint8_t {
  B,  // 000
  C,  // 001
  D,  // 010
  E,  // 011
  H,  // 100
  L,  // 101
  HL, // 110
  A,  // 111
};

enum class RegisterPair : uint8_t { BC, DE, HL, AF };

enum class OpTypes : uint8_t {
  LOAD = 0b00,
  REGISTER_OP = 0b01,
  ARITHMETIC = 0b10,
  MISC_EX = 0b11
};

enum class OpArithmetic : uint8_t {
  ADD = 0b000,
  ADC = 0b001,
  SUB = 0b010,
  SBC = 0b011,
  AND = 0b100,
  XOR = 0b101,
  OR = 0b110,
  CP = 0b111
};

uint16_t program_counter;
uint16_t stack_pointer;

std::byte registers_values[REGISTERS_COUNT];
std::byte memory[RAM_SIZE];

template <typename T> T get_reg_value(const Register reg) {
  const unsigned int index = static_cast<unsigned int>(reg);
  return static_cast<T>(registers_values[index]);
}

template <typename T> void set_reg_value(const Register reg, T value) {
  const unsigned int index = static_cast<unsigned int>(reg);
  registers_values[index] = static_cast<std::byte>(value);
}

void print_reg(const Register reg) {
  auto reg_value = get_reg_value<unsigned int>(reg);
  std::println("Value in {} : {}", static_cast<unsigned int>(reg), reg_value);
}

void reset() {
  program_counter = 0;
  stack_pointer = RAM_SIZE - 1;

  std::fill(registers_values, registers_values + REGISTERS_COUNT,
            static_cast<std::byte>(0));
  std::fill(memory, memory + RAM_SIZE, static_cast<std::byte>(0));
}

bool load_program(fs::path program_path) {
  std::ifstream file(program_path, std::ios::binary);

  if (!file) {
    return false;
  }

  file.seekg(0, std::ios::end);
  std::streamsize program_size = file.tellg();
  file.seekg(0, std::ios::beg);

  file.read(reinterpret_cast<char *>(memory), program_size);

  file.close();

  return true;
}

void handle_load_op(const uint8_t opcode) {
  const Register dest_reg = static_cast<Register>((opcode & 0b00111000) >> 3);

  const Register src_reg = static_cast<Register>((opcode & 0b00000111));

  if (src_reg == Register::HL) {
    program_counter++;

    set_reg_value(dest_reg, memory[program_counter]);
  }
}

bool handle_register_op(const uint8_t opcode) {
  const Register dest_reg = static_cast<Register>((opcode & 0b00111000) >> 3);
  const Register src_reg = static_cast<Register>((opcode & 0b00000111));

  if (src_reg == dest_reg && src_reg == Register::HL) { // HALT operation
    return true;
  } else {
    set_reg_value(dest_reg, get_reg_value<std::byte>(src_reg));
  }
  return false;
}

void handle_arithmetic_op(const uint8_t opcode) {
  const auto op = static_cast<OpArithmetic>((opcode & 0b00111000) >> 3);

  const auto reg = static_cast<Register>((opcode & 0b00000111));

  const auto dest_value = get_reg_value<uint8_t>(Register::A);
  const auto src_value = get_reg_value<uint8_t>(reg);

  switch (op) {
  case OpArithmetic::ADD:
    set_reg_value(Register::A, dest_value + src_value);
    break;
  }
}

int main(int argc, char **argv) {

  if (argc < 2) {
    std::println(std::cerr, "No program provided.");
    return EXIT_FAILURE;
  }

  fs::path program_path = argv[1];

  if (!fs::exists(program_path)) {
    std::println(std::cerr, "Binary {} not found", program_path.string());
    return EXIT_FAILURE;
  }

  std::println("Executing {}", program_path.string());

  reset();

  if (!load_program(program_path)) {
    std::println(std::cerr, "Error loading program.");
    return EXIT_FAILURE;
  }

  bool exit_loop = true;

  while (program_counter < RAM_SIZE && !exit_loop) {
    const uint8_t opcode = static_cast<uint8_t>(memory[program_counter]);

    const OpTypes optype = static_cast<OpTypes>((opcode & 0b11000000) >> 6);

    switch (optype) {
    case OpTypes::LOAD: {
      handle_load_op(opcode);
      break;
    }
    case OpTypes::REGISTER_OP:
      exit_loop = handle_register_op(opcode);
      break;
    case OpTypes::ARITHMETIC:
      handle_arithmetic_op(opcode);
      break;
    case OpTypes::MISC_EX:
      break;
    }

    program_counter++;
  }

  print_reg(Register::A);

  // load program
  std::ifstream file(program_path, std::ios::binary);

  if (!file) {
    return EXIT_FAILURE;
  }

  file.seekg(0, std::ios::end);
  std::streamsize program_size = file.tellg();
  file.seekg(0, std::ios::beg);

  uint8_t *program = new uint8_t[program_size];
  file.read((char *)program, program_size);
  file.close();

  Memory memory;
  CPU z80;
  DebugCard debugger;

  // flash memory
  memory.load_bytes(0x0000, program, program_size);

  delete[] program;

  MotherBoard mother_board(FREQ_4MHZ);
  mother_board.add_component(z80);
  mother_board.add_component(memory);
  mother_board.add_component(debugger);
  mother_board.run();

  return EXIT_SUCCESS;
}
