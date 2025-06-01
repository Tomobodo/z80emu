#include "machine_utils.hpp"

#include "machine/cpu/cpu.hpp"
#include "machine/memory.hpp"

#include <filesystem>
#include <fstream>

#include <doctest/doctest.h>

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

  constexpr unsigned long long FREQ = 4'000'000;
  MotherBoard motherboard(FREQ);

  auto cpu = motherboard.add_component<CPU>();

  auto memory = motherboard.add_component<Memory>();
  memory->load_bytes(0x0000, program.data(), program_size);

  program.clear();

  motherboard.reset();

  return motherboard;
}

void run_program(
    std::string program_path, unsigned long timeout_ms,
    std::function<void(unsigned long t_cycle, MotherBoard &motherboard,
                       bool halted, bool &exit)>
        on_step) {
  MotherBoard motherboard = machine_setup(program_path);
  auto cpu = motherboard.get_component<CPU>();

  bool clock = true;
  bool clock_prev = true;
  bool exit = false;

  unsigned long t_count = 0;

  auto start = std::chrono::steady_clock::now();

  while (!exit) {
    motherboard.clock(clock);
    clock = !clock_prev;
    if (clock && !clock_prev) {
      t_count++;
    }
    clock_prev = clock;

    on_step(t_count, motherboard, cpu->is_halted(), exit);

    auto current = std::chrono::steady_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(current - start);

    if (duration.count() > timeout_ms) {
      FAIL("Program timeout");
      exit = true;
    }
  }
}
