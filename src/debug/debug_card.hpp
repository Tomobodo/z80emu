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

private:
  static constexpr unsigned int PLOT_DATA_SIZE = 500;

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
};
