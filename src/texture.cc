#include <iostream>

#include "project.h"
#include "texture.h"

namespace purr {
  int Texture::LoadTexture(void * texturePtr) {
    Texture * texture = static_cast<Texture *>(texturePtr);
    SDLDisplay * display = Project::Instance()->Display();

    texture->sdlTexure = display->CreateSDLTextureFromImage(texture->filename);
    return 0;
  }

  Texture::Texture(std::string filename) : filename(filename) {
    sdlTexure = nullptr;
    loadingThread = nullptr;
  }

  void Texture::Load() {
    if (loadingThread != nullptr) {
      return;
    }


    loadingThread = SDL_CreateThread(
      Texture::LoadTexture,
      filename.c_str(),
      static_cast<void *>(this)
    );

    if (loadingThread == nullptr) {
      std::cerr << "SDL_CreateThread Error :" << SDL_GetError() << std::endl;
    }
  }
}
