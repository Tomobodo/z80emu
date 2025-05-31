#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <vector>

#include "control_bus.hpp"

class Component;

class MotherBoard {
public:
  MotherBoard(const MotherBoard &) = default;
  MotherBoard(MotherBoard &&) = default;
  auto operator=(const MotherBoard &) -> MotherBoard & = default;
  auto operator=(MotherBoard &&) -> MotherBoard & = default;
  explicit MotherBoard(unsigned int frequency);

  ~MotherBoard() = default;

  template <typename T>
  auto add_component() -> std::shared_ptr<T>
    requires(std::is_base_of_v<Component, T>)
  {
    auto component = std::make_shared<T>();
    component->set_mother_board(this);
    m_components.push_back(component);
    return component;
  }

  template <typename T> auto get_component() -> std::shared_ptr<T> {
    for (auto component : m_components) {
      if (auto casted_component = std::static_pointer_cast<T>(component)) {
        return casted_component;
      }
    }

    return nullptr;
  }

  void reset();

  void run();

  void clock(bool clock_active);

  void set_clock_frequency(unsigned long long frequency);

  void power_off();

  void set_clock_paused(const bool clock_paused) {
    m_clock_paused = clock_paused;
  }

  [[nodiscard]] auto get_emulation_lagging() const -> bool {
    return m_emulation_lagging;
  };

  [[nodiscard]] auto get_frequency() const -> unsigned long long {
    return m_frequency;
  };

  [[nodiscard]] auto get_control_bus() const -> uint16_t {
    return m_control_bus;
  }

private:
  void update();

  static constexpr unsigned int MAX_CATCHUP_CYCLES_NUM = 250'000;

  bool m_is_on = true;
  bool m_clock_phase = false;
  bool m_clock_paused = false;
  bool m_emulation_lagging = false;

  std::chrono::high_resolution_clock::time_point m_last_time;
  unsigned long long m_clock_time_acc{};
  unsigned long long m_clock_delay{};

  unsigned long long m_frequency{};

  std::vector<std::shared_ptr<Component>> m_components;

  ControlBus m_control_bus{};
  uint16_t m_address_bus{};
  uint8_t m_data_bus{};
};
