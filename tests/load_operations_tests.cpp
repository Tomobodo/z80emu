#include "machine/cpu/cpu.hpp"
#include "machine/cpu/registers.hpp"

#include "machine/mother_board.hpp"
#include <doctest/doctest.h>

#include "machine_utils.hpp"

constexpr unsigned long TIMEOUT = 1'000;

TEST_CASE("8 BIT IMMEDIATE") {

  run_program(
      "ld_immediate.bin", TIMEOUT,
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

TEST_CASE("REGISTER ADDRESSING") {
  run_program(
      "ld_register_addressing.bin", TIMEOUT,
      [](unsigned long t_cycle, MotherBoard &board, bool halted, bool &exit) {
        if (t_cycle == 31) {
          auto cpu = board.get_component<CPU>();

          CHECK(cpu->get_register(Register_8::A) == 0x24);
          CHECK(cpu->get_register(Register_8::B) == 0x24);
          CHECK(cpu->get_register(Register_8::C) == 0x24);
          CHECK(cpu->get_register(Register_8::D) == 0x24);
          CHECK(cpu->get_register(Register_8::E) == 0x24);
          CHECK(cpu->get_register(Register_8::H) == 0x24);
          CHECK(cpu->get_register(Register_8::L) == 0x24);
        }

        if (t_cycle == 62) {
          auto cpu = board.get_component<CPU>();

          CHECK(cpu->get_register(Register_8::A) == 0x47);
          CHECK(cpu->get_register(Register_8::B) == 0x47);
          CHECK(cpu->get_register(Register_8::C) == 0x47);
          CHECK(cpu->get_register(Register_8::D) == 0x47);
          CHECK(cpu->get_register(Register_8::E) == 0x47);
          CHECK(cpu->get_register(Register_8::H) == 0x47);
          CHECK(cpu->get_register(Register_8::L) == 0x47);
        }

        if (halted) {
          exit = true;
        }
      });
}
