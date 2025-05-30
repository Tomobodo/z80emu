#include <string>
#include <utility>

#include "machine/mother_board.hpp"

class MachineSetupException : public std::exception {
public:
  MachineSetupException(std::string message) : m_message(std::move(message)) {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    const char *ret = m_message.c_str();
    return ret;
  }

private:
  std::string m_message;
};

auto machine_setup(std::string program_path_str) -> MotherBoard;

void machine_half_clock_cycles(MotherBoard &mother_board,
                               unsigned long long half_cycles);
