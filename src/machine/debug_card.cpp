#include "debug_card.hpp"

#include <iostream>
#include <print>

void DebugCard::clock(bool clock_high) {
  std::println("Clock: {}", clock_high ? "HIGH" : "LOW");
  std::println("Data bus: {:#04X}", m_data_bus_in);
  std::println("Address bus: {:#06X}", m_address_bus_in);
  std::println("Control bus: {}",
               std::bitset<16>(m_control_bus_in).to_string());

  std::cin.ignore();
}

void DebugCard::reset() {}
