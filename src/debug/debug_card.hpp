#pragma once

#include "machine/component.hpp"

#include <SDL_events.h>
#include <SDL_render.h>
#include <imgui.h>

class DebugCard : public Component {
public:
  DebugCard();
  ~DebugCard();

  void update(double delta_time) override;

  void clock(bool clock_high) override;

  void reset() override;

private:
  void init();

  void deinit();

  void draw_ui();

  void draw_clock_section();
  void draw_control_bus_section();
  void draw_data_bus_section();
  void draw_mother_board_section();

private:
  static constexpr unsigned int PLOT_DATA_SIZE = 500;

  static constexpr ImVec4 RED_LED_OFF = ImVec4(0.85f, 0.3f, 0.2f, 0.4f);
  static constexpr ImVec4 GREEN_LED_OFF = ImVec4(0.3f, 0.7f, 0.2f, 0.4f);

  static constexpr ImVec4 RED_LED_ON = ImVec4(0.85f, 0.3f, 0.2f, 1.0f);
  static constexpr ImVec4 GREEN_LED_ON = ImVec4(0.3f, 0.7f, 0.2f, 1.0f);

  SDL_Event m_window_events;

  SDL_Window *m_window;
  SDL_Renderer *m_renderer;

  ImGuiContext *m_imgui_context;

  bool m_initialized;
  bool m_clock_high;

  bool m_clock_plot_data[PLOT_DATA_SIZE];
  uint16_t m_control_bus_plot_data[PLOT_DATA_SIZE];

  uint16_t m_control_bus_out_toggle;

  unsigned int m_plot_data_offset;

  int m_win_width, m_win_height;

  int m_mother_board_frequency;
};
