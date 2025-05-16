#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>

#include "debug/debug_card.hpp"

#include "machine/cpu/cpu.hpp"
#include "machine/cpu/z80_executor.hpp"
#include "machine/memory.hpp"
#include "machine/mother_board.hpp"

namespace fs = std::filesystem;

constexpr uint8_t REGISTERS_COUNT = 0b111;
constexpr uint16_t RAM_SIZE = 0xFFFF;

constexpr unsigned int FREQ = 4;

auto main(int argc, char *argv[]) -> int {
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
  z80.set_instruction_executor<Z80Executor>();
  DebugCard debugger(z80, memory);

  // flash memory
  memory.load_bytes(0x0000, program, program_size);

  delete[] program;

  MotherBoard mother_board(FREQ);
  mother_board.add_component(z80);
  mother_board.add_component(memory);
  mother_board.add_component(debugger);
  mother_board.run();

  return EXIT_SUCCESS;
}
