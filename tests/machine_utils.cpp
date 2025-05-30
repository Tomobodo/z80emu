#include "machine_utils.hpp"

#include "machine/cpu/cpu.hpp"
#include "machine/memory.hpp"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

auto machine_setup(std::string program_path_str) -> MotherBoard {
  fs::path program_path = program_path_str;

  if (!fs::exists(program_path)) {
    throw MachineSetupException(
        std::format("Program {} not found", program_path.string()));
  }

  std::ifstream file(program_path, std::ios::binary);

  if (!file) {
    throw MachineSetupException(
        std::format("Could not open file {}", program_path.string()));
  }

  file.seekg(0, std::ios::end);
  std::streamsize program_size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<uint8_t> program(program_size);
  file.read(std::bit_cast<char *>(program.data()), program_size);
  file.close();

  Memory memory;
  CPU z80;
  memory.load_bytes(0x0000, program.data(), program_size);

  program.clear();

  constexpr unsigned long long FREQ = 4'000'000;
  MotherBoard board(FREQ);
  board.add_component(z80);
  board.add_component(memory);
  return board;
}

void machine_half_clock_cycles(MotherBoard &mother_board,
                               unsigned long long half_cycles) {
  mother_board.reset();
  bool clock = true;
  for (unsigned long long i = 0; i < half_cycles; i++) {
    mother_board.clock(clock);
    clock = !clock;
  }
}
