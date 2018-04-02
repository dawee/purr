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
      SDL_mutex * mutex;
      std::string filename;
      Uint32 format;
      int access;
      int width;
      int height;

      static int LoadTexture(void *);

    public:
      void Load(SDLDisplay * display);
      bool IsLoaded();
      void Draw(SDLDisplay *, int, int, int, int, int, int);
      int Width();
      int Height();

      Texture(std::string);
      ~Texture();
  };
}

#endif
