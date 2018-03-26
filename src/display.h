#ifndef PURR_DISPLAY_H
#define PURR_DISPLAY_H

#include <SDL2/SDL.h>

namespace purr {
  class SDLDisplay {
    private:
      SDL_Window * window;
      SDL_Renderer * renderer;
      SDL_Texture * background;

    public:
      void Clear();
      void Hide();
      void Render();
      void DrawSDLTexture(SDL_Texture *, int, int, int, int);

      SDL_Texture * CreateSDLTextureFromImage(std::string);

      SDLDisplay();
      ~SDLDisplay();
  };
}

#endif
