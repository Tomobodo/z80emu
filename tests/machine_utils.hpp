#include <functional>
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

void run_program(
    std::string program_path, unsigned long timeout_ms,
    std::function<void(unsigned long t_cycle, MotherBoard &motherboard,
                       bool halted, bool &exit)>
        on_step);
