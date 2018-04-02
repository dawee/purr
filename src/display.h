#ifndef PURR_DISPLAY_H
#define PURR_DISPLAY_H

#include <string>
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
      void DrawSDLTexture(SDL_Texture *, SDL_Rect&, SDL_Rect&);
      SDL_Texture * CreateSDLTextureFromImage(std::string);

      SDLDisplay();
      ~SDLDisplay();
  };
}

#endif
