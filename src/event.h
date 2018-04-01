#ifndef PURR_EVENT_H
#define PURR_EVENT_H

#include <SDL2/SDL.h>

namespace purr {
  class Event {
    public:
      Event(SDL_Event& sdlEvent);
  };
}

#endif
