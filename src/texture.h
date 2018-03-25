#ifndef PURR_TEXTURE_H
#define PURR_TEXTURE_H

#include <iostream>
#include <SDL2/SDL.h>

namespace purr {
  class Texture {
    private:
      SDL_Texture * sdlTexure;
      SDL_Thread * loadingThread;
      std::string filename;

    public:
      static int LoadTexture(void *);

      void Load();

      Texture(std::string);
  };
}

#endif
