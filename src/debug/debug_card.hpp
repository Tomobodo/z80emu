#pragma once

#include "machine/component.hpp"

#include "SDL3/SDL_video.h"

class DebugCard : public Component {
public:
  DebugCard();

  void clock(bool clock_high) override;

  void reset() override;

private:

  SDL_Window* m_window;
};
