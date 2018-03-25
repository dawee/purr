#include <iostream>

#include "texture.h"

namespace purr {
  int Texture::LoadTexture(void * texturePtr) {
    Texture * texture = static_cast<Texture *>(texturePtr);
    SDL_Surface * bmp = SDL_LoadBMP(texture->filename.c_str());

    if (bmp == nullptr) {
    	std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
    	return 1;
    }

    //SDL_Texture * sdlTexure = SDL_CreateTextureFromSurface(ren, bmp);
    SDL_FreeSurface(bmp);

    // if (sdlTexure == nullptr){
    // 	std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
    // 	return 1;
    // }

    return 0;
  }

  Texture::Texture(std::string filename) : filename(filename) {
    sdlTexure = NULL;
    loadingThread = NULL;
  }

  void Texture::Load() {
    if (loadingThread == NULL) {
      return;
    }

    loadingThread = SDL_CreateThread(
      Texture::LoadTexture,
      filename.c_str(),
      static_cast<void *>(this)
    );
  }
}
