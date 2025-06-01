#include "machine/cpu/cpu.hpp"
#include "machine/cpu/registers.hpp"

#include "machine/mother_board.hpp"
#include <doctest/doctest.h>

#include "machine_utils.hpp"

constexpr unsigned long TIMEOUT = 1'000;

TEST_CASE("Load operations") {

  SUBCASE("8 BIT IMMEDIATE") {
    run_program(
        "test_programs/ld_immediate.bin", TIMEOUT,
        [](unsigned long t_cycle, MotherBoard &board, bool halted, bool &exit) {
          if (halted) {
            auto cpu = board.get_component<CPU>();

            CHECK(cpu->get_register(Register_8::A) == 0x05);
            CHECK(cpu->get_register(Register_8::B) == 0x15);
            CHECK(cpu->get_register(Register_8::C) == 0xFF);
            CHECK(cpu->get_register(Register_8::D) == 0x58);
            CHECK(cpu->get_register(Register_8::E) == 0xA5);
            CHECK(cpu->get_register(Register_8::H) == 0x93);
            CHECK(cpu->get_register(Register_8::L) == 0xAB);

            exit = true;
          }
        });
  }

  SUBCASE("8 BIT REGISTER ADDRESSING") {}
}
