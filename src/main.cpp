#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <span>

#include "debug/debug_card.hpp"

#include "machine/cpu/cpu.hpp"
#include "machine/memory.hpp"
#include "machine/mother_board.hpp"

namespace fs = std::filesystem;

constexpr uint8_t REGISTERS_COUNT = 0b111;
constexpr uint16_t RAM_SIZE = 0xFFFF;

constexpr unsigned int FREQ = 4;

auto main(int argc, char *argv[]) -> int {
  std::span<char *> args(argv, argc);

  if (args.size() < 2) {
    std::println(std::cerr, "No program provided.");
    return EXIT_FAILURE;
  }

  fs::path program_path = args[1];

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

  std::vector<uint8_t> program(program_size);
  file.read(std::bit_cast<char *>(program.data()), program_size);
  file.close();

  // flash memory

  MotherBoard mother_board(FREQ);
  auto z80 = mother_board.add_component<CPU>();

  auto memory = mother_board.add_component<Memory>();
  memory->load_bytes(0x0000, program.data(), program_size);
  program.clear();

  mother_board.add_component<DebugCard>();

  mother_board.run();

  return EXIT_SUCCESS;
}
