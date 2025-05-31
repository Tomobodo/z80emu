#include "debug_card.hpp"
#include "SDL3/SDL_render.h"
#include "imgui.h"
#include "machine/control_bus.hpp"
#include "machine/cpu/registers.hpp"
#include "utils/bit_operations.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <print>

#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_sdlrenderer3.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

DebugCard::~DebugCard() { deinit(); }

void DebugCard::clock(bool clock_active) {
  if (m_half_step || (m_step && clock_active)) {
    set_paused(true);

    m_half_step = false;
    m_step = false;
  }

  m_clock_active = clock_active;
  m_control_bus_out = m_control_bus_out_toggle;
}

struct get_control_bus_plot_value_params {
  int bit;
  std::array<uint16_t, DebugCard::PLOT_DATA_SIZE> *data;
};

auto get_control_bus_plot_value(void *data_in, int index) -> float {
  auto *const params =
      static_cast<get_control_bus_plot_value_params *>(data_in);
  uint16_t value = params->data->at(index);
  const auto plot_value = static_cast<float>(1 & (value >> params->bit));
  return 1.F - plot_value;
}

void DebugCard::update(double delta_time) {
  if (m_powered_off) {
    deinit();
    m_mother_board->power_off();
    return;
  }

  if (!m_initialized)
    return;

  // update data
  if (!m_paused) {
    int data_index = m_plot_data_offset % PLOT_DATA_SIZE;
    m_clock_plot_data.at(data_index) = m_clock_active;
    m_control_bus_plot_data.at(data_index) = m_mother_board->get_control_bus();

    m_plot_data_offset++;
  }

  // render

  ImGuiIO &imgui_io = ImGui::GetIO();
  bool show_demo_window = true;

  while (SDL_PollEvent(&m_window_events) != 0) {
    ImGui_ImplSDL3_ProcessEvent(&m_window_events);
    if (m_window_events.type == SDL_EVENT_QUIT) {
      deinit();
      return;
    }
  }

  SDL_GetWindowSize(m_window, &m_win_width, &m_win_height);

  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  draw_ui();

  ImGui::Render();
  SDL_SetRenderScale(m_renderer, imgui_io.DisplayFramebufferScale.x,
                     imgui_io.DisplayFramebufferScale.y);
  SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, UINT8_MAX);
  SDL_RenderClear(m_renderer);

  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_renderer);

  SDL_RenderPresent(m_renderer);
}

auto get_clock_plot_value(void *data_in, int index) -> float {
  auto *data =
      static_cast<std::array<bool, DebugCard::PLOT_DATA_SIZE> *>(data_in);
  return data->at(index) ? 1.0 : 0.0;
}

void DebugCard::reset() {
  deinit();
  init();
}

void DebugCard::on_added() {
  m_memory = m_mother_board->get_component<Memory>();
  m_cpu = m_mother_board->get_component<CPU>();
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

  m_clock_plot_data.fill(false);
  m_control_bus_plot_data.fill(0);

  // SDL / Imgui
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    std::println(std::cerr, "Error: SDL could not initialize : {}",
                 SDL_GetError());
  }

  auto window_flags = static_cast<SDL_WindowFlags>(
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

  m_window =
      SDL_CreateWindow("CPU Debug", WINDOW_WIDTH, WINDOW_HEIGHT, window_flags);

  if (m_window == nullptr) {
    std::println(std::cerr, "Error: Could not init debugger window : {}",
                 SDL_GetError());
    return;
  }

  m_renderer = SDL_CreateRenderer(m_window, nullptr);
  if (!m_renderer) {
    std::println(std::cerr, "Error: SDL could not initilize a renderer : {}",
                 SDL_GetError());
    return;
  }

  IMGUI_CHECKVERSION();
  m_imgui_context = ImGui::CreateContext();
  ImGui::SetCurrentContext(m_imgui_context);

  ImGui::StyleColorsDark();

  ImGui_ImplSDL3_InitForSDLRenderer(m_window, m_renderer);
  ImGui_ImplSDLRenderer3_Init(m_renderer);
}

void DebugCard::deinit() {
  if (!m_initialized)
    return;

  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
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
    const int ITEM_WIDTH = 130;
    ImGui::PushItemWidth(ITEM_WIDTH);

    if (ImGui::InputScalar("Frequency", ImGuiDataType_U64,
                           &m_mother_board_frequency, nullptr, nullptr,
                           "%llu")) {
      if (m_mother_board_frequency < 1) {
        m_mother_board_frequency = 1;
      }

      if (m_mother_board_frequency > MAX_MOTHER_BOARD_FREQUENCY) {
        m_mother_board_frequency = MAX_MOTHER_BOARD_FREQUENCY;
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

    ImGui::PlotLines("Signal", &get_clock_plot_value, &m_clock_plot_data,
                     PLOT_DATA_SIZE, m_plot_data_offset, nullptr, 0, 1);
  }
}

void DebugCard::draw_control_bus_section() {
  if (ImGui::CollapsingHeader("Control Bus", ImGuiTreeNodeFlags_DefaultOpen)) {
    for (int bit = 0; bit < CONTROL_BUS_BITS; bit++) {
      const std::string pin_name = CONTROL_BUS_PIN_NAMES.at(bit);
      get_control_bus_plot_value_params params = {
          .bit = bit, .data = &m_control_bus_plot_data};
      ImGui::PlotLines(std::format("##{}", pin_name).c_str(),
                       &get_control_bus_plot_value, &params, PLOT_DATA_SIZE,
                       m_plot_data_offset, nullptr, 0, 1);

      bool bit_on = (m_control_bus_out_toggle >> bit) & 1 ||
                    get_bit(m_control_bus_in, bit);

      if (bit_on) {
        ImGui::PushStyleColor(ImGuiCol_Button, GREEN_LED_ON);
      } else {
        ImGui::PushStyleColor(ImGuiCol_Button, RED_LED_OFF);
      }

      ImGui::SameLine();
      const char *button_name = CONTROL_BUS_PIN_NAMES.at(bit);

      if (CONTROL_BUS_PINS.at(bit) == ControlBusPin::RESET) {
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
    const int last_data_bus_bit = 7;
    for (int bit = last_data_bus_bit; bit >= 0; bit--) {
      bool on = get_bit(m_data_bus_in, bit);

      ImVec4 led_color = on ? GREEN_LED_ON : GREEN_LED_OFF;
      ImGui::PushStyleColor(ImGuiCol_Button, led_color);

      if (bit < last_data_bus_bit)
        ImGui::SameLine();

      ImGui::Button(std::format("##data_bit{}", bit).c_str(),
                    ImVec2(LED_SIZE, LED_SIZE));
      ImGui::PopStyleColor();
    }

    ImGui::TextUnformatted(std::format("Hex: 0x{:02X}", m_data_bus_in).c_str());
    ImGui::SameLine();
    ImGui::TextUnformatted(std::format("Dec: {}", m_data_bus_in).c_str());
    ImGui::SameLine();
    ImGui::TextUnformatted(
        std::format("Char: {:c}", static_cast<char>(m_data_bus_in)).c_str());
  }
}

void DebugCard::draw_address_bus_section() {
  if (ImGui::CollapsingHeader("Address Bus", ImGuiTreeNodeFlags_DefaultOpen)) {
    const int last_address_bus_bit = 15;
    for (int bit = last_address_bus_bit; bit >= 0; bit--) {
      bool on = get_bit(m_address_bus_in, bit);

      ImVec4 led_color = on ? GREEN_LED_ON : GREEN_LED_OFF;
      ImGui::PushStyleColor(ImGuiCol_Button, led_color);

      if (bit < last_address_bus_bit)
        ImGui::SameLine();

      ImGui::Button(std::format("##address_bit{}", bit).c_str(),
                    ImVec2(LED_SIZE, LED_SIZE));
      ImGui::PopStyleColor();
    }
  }

  ImGui::TextUnformatted(
      std::format("Hex: 0x{:02X}", m_address_bus_in).c_str());
  ImGui::SameLine();
  ImGui::TextUnformatted(std::format("Dec: {}", m_address_bus_in).c_str());
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
      m_powered_off = true;
    }
    ImGui::PopStyleColor();
  }
}

void DebugCard::draw_8b_register(Register_8 reg) {
  ImGui::TableNextColumn();

  ImGui::Selectable(std::format("{}", register_8_to_name(reg)).c_str(), true);

  const auto value = m_cpu->get_register(reg);

  ImGui::TextUnformatted(std::format("H: 0x{:02X}", value).c_str());
  ImGui::SameLine();
  ImGui::TextUnformatted(std::format("D: {}", value).c_str());

  std::string binary = std::format("{:08b}", value);
  binary.insert(4, " ");
  binary = "B: " + binary;
  ImGui::TextUnformatted(binary.c_str());
  ImGui::SameLine();
  ImGui::TextUnformatted(
      std::format("C: {:c}", static_cast<char>(value)).c_str());
}

void DebugCard::draw_16b_register(Register_16 reg) {
  ImGui::TableNextColumn();

  ImGui::Selectable(std::format("{}", register_16_to_name(reg)).c_str(), true);

  const auto value = m_cpu->get_register(reg);

  ImGui::TextUnformatted(std::format("H: 0x{:04X}", value).c_str());
  ImGui::SameLine();
  ImGui::TextUnformatted(std::format("D: {}", value).c_str());

  std::string binary = std::format("{:016b}", value).c_str();
  const int insert_pos_1 = 4, insert_pos_2 = 9, insert_pos_3 = 14;
  binary.insert(insert_pos_1, " ");
  binary.insert(insert_pos_2, " ");
  binary.insert(insert_pos_3, " ");
  binary = "B: " + binary;
  ImGui::TextUnformatted(binary.c_str());
  ImGui::SameLine();
  ImGui::TextUnformatted(
      std::format("C: {:c}", static_cast<char>(value)).c_str());
}

void DebugCard::draw_cpu_section() {
  if (ImGui::CollapsingHeader("8bit Registers",
                              ImGuiTreeNodeFlags_DefaultOpen)) {

    ImGui::BeginTable("8 bit", 4,
                      ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);

    for (auto i : REGISTER_8_DEBUG_ORDER) {
      draw_8b_register(i);
    }

    ImGui::EndTable();
  }

  if (ImGui::CollapsingHeader("16bit Registers",
                              ImGuiTreeNodeFlags_DefaultOpen)) {

    ImGui::BeginTable("16 bit", 2,
                      ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);

    for (auto i : REGISTER_16_DEBUG_ORDER) {
      draw_16b_register(i);
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

    const int scroll_y = static_cast<int>(ImGui::GetScrollY());

    uint16_t start_index = std::clamp(static_cast<int>(scroll_y / ITEMS_HEIGHT),
                                      0, ITEMS_COUNT - MEMORY_READ_SIZE);

    const uint16_t end_index = start_index + MEMORY_READ_SIZE;

    ImGui::SetCursorPosY(ITEMS_HEIGHT);

    ImGui::Dummy(ImVec2(0, (ITEMS_COUNT + 1) * ITEMS_HEIGHT));

    ImGui::SetCursorPosY(static_cast<float>(start_index * ITEMS_HEIGHT +
                                            scroll_y % ITEMS_HEIGHT));

    ImGui::BeginTable("Memory", 2,
                      ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);

    ImGui::TableNextColumn();
    ImGui::Selectable("Address", true);

    ImGui::TableNextColumn();
    ImGui::Selectable("Value", true);

    for (int i = 0; i <= MEMORY_READ_SIZE; i++) {
      const uint16_t address = start_index + i;
      const uint8_t value = m_memory->read_address(address);

      ImU32 cell_bg IM_COL32(0, 0, 0, 0);

      if (address == m_address_bus_in) {
        cell_bg = IM_COL32(0, 255, 0, 80);
      }

      ImGui::TableNextColumn();
      ImGui::TextUnformatted(std::format("0x{:04X}", address).c_str());

      ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg);

      ImGui::TableNextColumn();
      ImGui::TextUnformatted(std::format("0x{:02X}", value).c_str());

      ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg);
    }

    ImGui::EndTable();

    ImGui::EndChild();
  }
}
