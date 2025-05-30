#include "machine/cpu/cpu.hpp"
#include "machine/cpu/registers.hpp"

#include "machine/mother_board.hpp"
#include <doctest/doctest.h>

#include "machine_utils.hpp"

TEST_CASE("Load operations") {
  MotherBoard mother_board = machine_setup("test_programs_src/ld_test.bin");
  auto cpu = mother_board.get_component<CPU>();

  const auto nb_cycles = 1'000;
  machine_half_clock_cycles(mother_board, nb_cycles);

  REQUIRE(cpu != nullptr);

  SUBCASE("DIRECT 8 BIT") {
    CHECK(cpu->get_register(Register_8::A) == 0x05);
    CHECK(cpu->get_register(Register_8::B) == 0x15);
    CHECK(cpu->get_register(Register_8::C) == 0xFF);
    CHECK(cpu->get_register(Register_8::D) == 0x58);
    CHECK(cpu->get_register(Register_8::E) == 0xA5);
  }
}
