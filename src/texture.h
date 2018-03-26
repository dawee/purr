#ifndef PURR_TEXTURE_H
#define PURR_TEXTURE_H

#include <iostream>
#include <SDL2/SDL.h>

#include "display.h"

namespace purr {
  class Texture {
    private:
      SDL_Texture * sdlTexture;
      SDL_Thread * loadingThread;
      std::string filename;
      Uint32 format;
      int access;
      int width;
      int height;

    public:
      static int LoadTexture(void *);

      void Load();
      bool IsLoaded();
      void Draw(SDLDisplay *);

      Texture(std::string);
  };
}

#endif
