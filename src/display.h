#ifndef PURR_DISPLAY_H
#define PURR_DISPLAY_H

#include <SDL2/SDL.h>

namespace purr {
  class Display {
    private:
      SDL_Window * window;
      SDL_Renderer * renderer;
      SDL_Texture * background;

    public:
      void Draw();
      void Hide();

      Display();
      ~Display();
  };
}

#endif
