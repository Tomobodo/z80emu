#include "debug_card.hpp"
#include "SDL3/SDL_init.h"

#include <iostream>
#include <print>

DebugCard::DebugCard() { m_window = nullptr; }

void DebugCard::clock(bool clock_high) {
  std::println("Clock: {}", clock_high ? "HIGH" : "LOW");
  std::println("Data bus: {:#04X}", m_data_bus_in);
  std::println("Address bus: {:#06X}", m_address_bus_in);
  std::println("Control bus: {}",
               std::bitset<16>(m_control_bus_in).to_string());

  std::cin.ignore();
}

void DebugCard::reset() {
  if (m_window != nullptr) {
    SDL_DestroyWindow(m_window);
  }

  m_window = nullptr;
}
