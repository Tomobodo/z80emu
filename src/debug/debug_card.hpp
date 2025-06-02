#pragma once

#include "machine/component.hpp"
#include "machine/cpu/cpu.hpp"
#include "machine/cpu/registers.hpp"
#include "machine/memory.hpp"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <imgui.h>

class DebugCard : public Component {
public:
  DebugCard(MotherBoard *mother_board);
  DebugCard(const DebugCard &) = default;
  DebugCard(DebugCard &&) = delete;
  auto operator=(const DebugCard &) -> DebugCard & = default;
  auto operator=(DebugCard &&) -> DebugCard & = delete;
  ~DebugCard() override;

  void update(double delta_time) override;

  void clock(bool clock_high) override;

  void reset() override;

  // ----- STATIC CONSTANTS -----

  static constexpr int PLOT_DATA_SIZE = 500;

private:
  // ----- METHODS -----

  void init();

  void init_initial_state();
  void init_rendering();

  void deinit();

  void set_paused(bool paused);

  void draw_ui();

  void draw_clock_section();
  void draw_control_bus_section();
  void draw_data_bus_section();
  void draw_address_bus_section();
  void draw_mother_board_section();
  void draw_8b_register(Register_8 reg);
  void draw_16b_register(Register_16 reg);
  void draw_cpu_section();
  void draw_memory_section();

  // ----- STATIC FIELDS -----

  static constexpr int WINDOW_WIDTH = 1600;
  static constexpr int WINDOW_HEIGHT = 720;

  static constexpr unsigned long long MAX_MOTHER_BOARD_FREQUENCY = 4'000'000;

  static constexpr unsigned int CONTROL_BUS_BITS = 13;

  static constexpr int LED_SIZE = 16;

  static constexpr ImVec4 RED_LED_OFF = ImVec4(0.85f, 0.3f, 0.2f, 0.4f);
  static constexpr ImVec4 GREEN_LED_OFF = ImVec4(0.3f, 0.7f, 0.2f, 0.4f);

  static constexpr ImVec4 RED_LED_ON = ImVec4(0.85f, 0.3f, 0.2f, 1.0f);
  static constexpr ImVec4 GREEN_LED_ON = ImVec4(0.3f, 0.7f, 0.2f, 1.0f);

  static constexpr std::array<Register_8, 18> REGISTER_8_DEBUG_ORDER = {
      Register_8::A, Register_8::F, Register_8::A_P, Register_8::F_P,
      Register_8::B, Register_8::C, Register_8::B_P, Register_8::C_P,
      Register_8::D, Register_8::E, Register_8::D_P, Register_8::E_P,
      Register_8::H, Register_8::L, Register_8::H_P, Register_8::L_P,
      Register_8::I, Register_8::R};

  static constexpr std::array<Register_16, 10> REGISTER_16_DEBUG_ORDER = {
      Register_16::BC, Register_16::BC_P, Register_16::DE, Register_16::DE_P,
      Register_16::HL, Register_16::HL_P, Register_16::IX, Register_16::IY,
      Register_16::SP, Register_16::PC};

  // ----- FIELDS -----

  int m_win_width{}, m_win_height{};

  SDL_Event m_window_events{};

  SDL_Window *m_window{};
  SDL_Renderer *m_renderer{};

  ImGuiContext *m_imgui_context{};

  bool m_initialized{};
  bool m_clock_active{};

  bool m_paused{};
  bool m_step{};
  bool m_half_step{};

  bool m_powered_off{};

  std::array<bool, PLOT_DATA_SIZE> m_clock_plot_data{};
  std::array<uint16_t, PLOT_DATA_SIZE> m_control_bus_plot_data{};
  int m_plot_data_offset{};

  uint16_t m_control_bus_out_toggle{};

  unsigned long long m_mother_board_frequency{};

  std::shared_ptr<CPU> m_cpu{};
  std::shared_ptr<Memory> m_memory{};

  MotherBoard *m_mother_board;
};
