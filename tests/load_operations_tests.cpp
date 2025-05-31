#include "machine/cpu/cpu.hpp"
#include "machine/cpu/registers.hpp"

#include "machine/mother_board.hpp"
#include <doctest/doctest.h>

#include "machine_utils.hpp"

TEST_CASE("Load operations") {

  SUBCASE("DIRECT 8 BIT") {
    MotherBoard board = run_program("test_programs/ld_test_direct.bin");
    auto cpu = board.get_component<CPU>();

    CHECK(cpu->get_register(Register_8::A) == 0x05);
    CHECK(cpu->get_register(Register_8::B) == 0x15);
    CHECK(cpu->get_register(Register_8::C) == 0xFF);
    CHECK(cpu->get_register(Register_8::D) == 0x58);
    CHECK(cpu->get_register(Register_8::E) == 0xA5);
  }
}
