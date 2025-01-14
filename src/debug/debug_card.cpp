#include "debug_card.hpp"
#include "imgui.h"
#include "machine/control_bus.hpp"
#include "machine/registers.hpp"
#include "utils/bit_operations.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <print>

#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>

#include <SDL.h>
#include <SDL_video.h>

DebugCard::DebugCard(CPU &cpu, Memory &memory) : m_cpu(cpu), m_memory(memory) {
  m_window = nullptr;
  m_initialized = false;
}

DebugCard::~DebugCard() { deinit(); }

void DebugCard::clock(bool clock_high) {
  if (m_half_step || (m_step && clock_high)) {
    set_paused(true);

    m_half_step = false;
    m_step = false;
  }

  m_clock_high = clock_high;
  m_control_bus_out = m_control_bus_out_toggle;
}

struct get_control_bus_plot_value_params {
  int bit;
  uint16_t *data;
};

float get_control_bus_plot_value(void *data_in, int index) {
  auto params = static_cast<get_control_bus_plot_value_params *>(data_in);
  uint16_t value = params->data[index];
  return (value >> params->bit) & 1;
}

void DebugCard::update(double delta_time) {
  if (!m_initialized)
    return;

  // update data
  if (!m_paused) {
    unsigned int data_index = m_plot_data_offset % PLOT_DATA_SIZE;
    m_clock_plot_data[data_index] = m_clock_high;
    m_control_bus_plot_data[data_index] = m_control_bus_in;

    m_plot_data_offset++;
  }

  // render

  ImGuiIO &io = ImGui::GetIO();
  bool show_demo_window = true;

  while (SDL_PollEvent(&m_window_events) != 0) {
    ImGui_ImplSDL2_ProcessEvent(&m_window_events);
    if (m_window_events.type == SDL_QUIT) {
      deinit();
      return;
    }
  }

  SDL_GetWindowSize(m_window, &m_win_width, &m_win_height);

  ImGui_ImplSDLRenderer2_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  draw_ui();

  ImGui::Render();
  SDL_RenderSetScale(m_renderer, io.DisplayFramebufferScale.x,
                     io.DisplayFramebufferScale.y);
  SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
  SDL_RenderClear(m_renderer);

  ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_renderer);

  SDL_RenderPresent(m_renderer);
}

float get_clock_plot_value(void *data_in, int index) {
  return static_cast<bool *>(data_in)[index] ? 1.0 : 0.0;
}

void DebugCard::reset() {
  deinit();
  init();
}

// ----- PRIVATE ----- //

void DebugCard::init() {
  init_initial_state();

  init_rendering();

  m_initialized = true;
}

void DebugCard::init_initial_state() {
  set_paused(true);

  m_mother_board_frequency = m_mother_board->get_frequency();
}

void DebugCard::init_rendering() {
  // plot data
  m_plot_data_offset = 0;

  std::fill(m_clock_plot_data, m_clock_plot_data + PLOT_DATA_SIZE, false);
  std::fill(m_control_bus_plot_data, m_control_bus_plot_data + PLOT_DATA_SIZE,
            0);

  // SDL / Imgui
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    std::println(std::cerr, "Error: SDL could not initialize : {}",
                 SDL_GetError());
  }

  SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  m_window = SDL_CreateWindow("CPU Debug", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, 1600, 720, window_flags);

  if (m_window == nullptr) {
    std::println(std::cerr, "Error: Could not init debugger window : {}",
                 SDL_GetError());
    return;
  }

  m_renderer = SDL_CreateRenderer(
      m_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
  if (!m_renderer) {
    std::println(std::cerr, "Error: SDL could not initilize a renderer : {}",
                 SDL_GetError());
    return;
  }

  IMGUI_CHECKVERSION();
  m_imgui_context = ImGui::CreateContext();
  ImGui::SetCurrentContext(m_imgui_context);

  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
  ImGui_ImplSDLRenderer2_Init(m_renderer);
}

void DebugCard::deinit() {
  if (!m_initialized)
    return;

  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext(m_imgui_context);

  SDL_DestroyRenderer(m_renderer);
  SDL_DestroyWindow(m_window);
  SDL_Quit();

  m_initialized = false;

  // quit app
  m_mother_board->power_off();
}

void DebugCard::set_paused(bool paused) {
  m_paused = paused;
  m_mother_board->set_clock_paused(paused);
}

void DebugCard::draw_ui() {
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(static_cast<float>(m_win_width),
                                  static_cast<float>(m_win_height)));

  ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoDecoration);

  ImGui::Columns(3);

  draw_clock_section();
  draw_control_bus_section();
  draw_data_bus_section();
  draw_address_bus_section();
  draw_mother_board_section();

  ImGui::NextColumn();

  draw_cpu_section();

  ImGui::NextColumn();

  draw_memory_section();

  // Mother board
  ImGui::End();
}

void DebugCard::draw_clock_section() {
  if (ImGui::CollapsingHeader("Clock", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::PushItemWidth(130);

    if (ImGui::InputInt("Frequency", &m_mother_board_frequency)) {
      if (m_mother_board_frequency < 1) {
        m_mother_board_frequency = 1;
      }

      if (m_mother_board_frequency > 4'000'000) {
        m_mother_board_frequency = 4'000'000;
      }

      m_mother_board->set_clock_frequency(m_mother_board_frequency);
    }

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button,
                          m_paused ? GREEN_LED_ON : GREEN_LED_OFF);
    if (ImGui::Button("Paused")) {
      set_paused(!m_paused);
    }
    ImGui::PopStyleColor();

    ImGui::SameLine();
    if (ImGui::Button("Step")) {
      m_step = true;
      set_paused(false);
    }

    ImGui::SameLine();
    if (ImGui::Button("Half Step")) {
      m_half_step = true;
      set_paused(false);
    }

    ImGui::PopItemWidth();

    ImGui::PlotLines("Signal", &get_clock_plot_value, m_clock_plot_data,
                     PLOT_DATA_SIZE, m_plot_data_offset, 0, 0, 1);
  }
}

void DebugCard::draw_control_bus_section() {
  if (ImGui::CollapsingHeader("Control Bus", ImGuiTreeNodeFlags_DefaultOpen)) {
    for (int bit = 0; bit < 13; bit++) {

      get_control_bus_plot_value_params params = {bit, m_control_bus_plot_data};
      ImGui::PlotLines(std::format("##{}", CONTROL_BUS_PIN_NAMES[bit]).c_str(),
                       &get_control_bus_plot_value, &params, PLOT_DATA_SIZE,
                       m_plot_data_offset, 0, 0, 1);

      bool bit_on = (m_control_bus_out_toggle >> bit) & 1;

      if (bit_on) {
        ImGui::PushStyleColor(ImGuiCol_Button, GREEN_LED_ON);
      } else {
        ImGui::PushStyleColor(ImGuiCol_Button, RED_LED_OFF);
      }

      ImGui::SameLine();
      const char *button_name = CONTROL_BUS_PIN_NAMES[bit];

      if (CONTROL_BUS_PINS[bit] == ControlBusPin::RESET) {
        ImGui::Button(button_name);

        if (ImGui::IsItemActive()) {
          set_bit(m_control_bus_out_toggle, bit, true);
        } else {
          set_bit(m_control_bus_out_toggle, bit, false);
        }
      } else {
        if (ImGui::Button(button_name)) {
          set_bit(m_control_bus_out_toggle, bit, !bit_on);
        }
      }

      ImGui::PopStyleColor();
    }
  }
}

void DebugCard::draw_data_bus_section() {
  if (ImGui::CollapsingHeader("Data Bus", ImGuiTreeNodeFlags_DefaultOpen)) {
    for (int bit = 7; bit >= 0; bit--) {
      bool on = get_bit(m_data_bus_in, bit);

      ImVec4 led_color = on ? GREEN_LED_ON : GREEN_LED_OFF;
      ImGui::PushStyleColor(ImGuiCol_Button, led_color);

      if (bit < 7)
        ImGui::SameLine();

      ImGui::Button(std::format("##data_bit{}", bit).c_str(), ImVec2(16, 16));
      ImGui::PopStyleColor();
    }

    ImGui::Text("Hex: 0x%02X", m_data_bus_in);
    ImGui::SameLine();
    ImGui::Text("Dec: %d", m_data_bus_in);
    ImGui::SameLine();
    ImGui::Text("Char: %c", m_data_bus_in);
  }
}

void DebugCard::draw_address_bus_section() {
  if (ImGui::CollapsingHeader("Address Bus", ImGuiTreeNodeFlags_DefaultOpen)) {
    for (int bit = 15; bit >= 0; bit--) {
      bool on = get_bit(m_address_bus_in, bit);

      ImVec4 led_color = on ? GREEN_LED_ON : GREEN_LED_OFF;
      ImGui::PushStyleColor(ImGuiCol_Button, led_color);

      if (bit < 15)
        ImGui::SameLine();

      ImGui::Button(std::format("##address_bit{}", bit).c_str(),
                    ImVec2(16, 16));
      ImGui::PopStyleColor();
    }
  }

  ImGui::Text("Hex: 0x%02X", m_address_bus_in);
  ImGui::SameLine();
  ImGui::Text("Dec: %d", m_address_bus_in);
}

void DebugCard::draw_mother_board_section() {
  if (ImGui::CollapsingHeader("Mother Board", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImVec4 lag_led_color =
        m_mother_board->get_emulation_lagging() ? RED_LED_ON : RED_LED_OFF;
    ImGui::PushStyleColor(ImGuiCol_Button, lag_led_color);
    ImGui::Button("Emulation is lagging !");
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Button, RED_LED_ON);
    if (ImGui::Button("Power")) {
      deinit();
      m_mother_board->power_off();
    }
    ImGui::PopStyleColor();
  }
}

void DebugCard::draw_8b_register(Register_8 reg) {
  ImGui::TableNextColumn();

  ImGui::Selectable(std::format("{}", register_8_to_name(reg)).c_str(), true);

  const auto value = m_cpu.get_register(reg);

  ImGui::Text("H: 0x%02X", value);
  ImGui::SameLine();
  ImGui::Text("D: %d", value);

  std::string binary = std::format("{:08b}", value);
  binary.insert(4, " ");
  ImGui::Text("B: %s", binary.c_str());
  ImGui::SameLine();
  ImGui::Text("C: %c", value);
}

void DebugCard::draw_16b_register(Register_16 reg) {
  ImGui::TableNextColumn();

  ImGui::Selectable(std::format("{}", register_16_to_name(reg)).c_str(), true);

  const auto value = m_cpu.get_register(reg);

  ImGui::Text("H: 0x%04X", value);
  ImGui::SameLine();
  ImGui::Text("D: %d", value);

  std::string binary = std::format("{:016b}", value).c_str();
  binary.insert(4, " ");
  binary.insert(9, " ");
  binary.insert(14, " ");
  ImGui::Text("B: %s", binary.c_str());
  ImGui::SameLine();
  ImGui::Text("C: %c", value);
}

void DebugCard::draw_cpu_section() {
  if (ImGui::CollapsingHeader("8bit Registers",
                              ImGuiTreeNodeFlags_DefaultOpen)) {

    ImGui::BeginTable("8 bit", 4,
                      ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);

    for (int i = 0; i < REGISTER_8_COUNT; i++) {
      draw_8b_register(REGISTER_8_DEBUG_ORDER[i]);
    }

    ImGui::EndTable();
  }

  if (ImGui::CollapsingHeader("16bit Registers",
                              ImGuiTreeNodeFlags_DefaultOpen)) {

    ImGui::BeginTable("16 bit", 2,
                      ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);

    for (int i = 0; i < REGISTER_16_COUNT; i++) {
      draw_16b_register(REGISTER_16_DEBUG_ORDER[i]);
    }

    ImGui::EndTable();
  }
}

void DebugCard::draw_memory_section() {
  if (ImGui::CollapsingHeader("Memory", ImGuiTreeNodeFlags_DefaultOpen)) {
    constexpr uint16_t MEMORY_READ_SIZE = 32;
    constexpr unsigned int ITEMS_HEIGHT = 18;
    constexpr uint16_t ITEMS_COUNT = 0xFFFF;

    ImGui::BeginChild("MemoryScrollArea",
                      ImVec2(0, (1 + ITEMS_HEIGHT) * MEMORY_READ_SIZE), true,
                      ImGuiWindowFlags_AlwaysVerticalScrollbar);

    const int scroll_y = ImGui::GetScrollY();

    uint16_t start_index = std::clamp(static_cast<int>(scroll_y / ITEMS_HEIGHT),
                                      0, ITEMS_COUNT - MEMORY_READ_SIZE);

    const uint16_t end_index = start_index + MEMORY_READ_SIZE;
    const uint8_t *memory_address = m_memory.get_address(start_index);

    ImGui::SetCursorPosY(ITEMS_HEIGHT);

    ImGui::Dummy(ImVec2(0, (ITEMS_COUNT + 1) * ITEMS_HEIGHT));

    ImGui::SetCursorPosY(start_index * ITEMS_HEIGHT + scroll_y % ITEMS_HEIGHT);

    ImGui::BeginTable("Memory", 2,
                      ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);

    ImGui::TableNextColumn();
    ImGui::Selectable("Address", true);

    ImGui::TableNextColumn();
    ImGui::Selectable("Value", true);

    for (int i = 0; i <= MEMORY_READ_SIZE; i++) {
      const uint16_t address = start_index + i;
      const uint8_t value = *(memory_address + i);

      ImU32 cell_bg IM_COL32(0, 0, 0, 0);

      if (address == m_address_bus_in) {
        cell_bg = IM_COL32(0, 255, 0, 80);
      }

      ImGui::TableNextColumn();
      ImGui::Text("0x%04X", address);

      ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg);

      ImGui::TableNextColumn();
      ImGui::Text("0x%02X", value);

      ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg);
    }

    ImGui::EndTable();

    ImGui::EndChild();
  }
}
