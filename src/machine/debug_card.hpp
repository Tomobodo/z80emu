#pragma once

#include "component.hpp"

class DebugCard : public Component {
public:
  void clock(bool clock_high) override;

  void reset() override;

private:
};
