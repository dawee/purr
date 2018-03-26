#include <iostream>

#include "project.h"
#include "texture.h"

namespace purr {
  int Texture::LoadTexture(void * texturePtr) {
    Texture * texture = static_cast<Texture *>(texturePtr);
    SDLDisplay * display = Project::Instance()->Display();

    SDL_Texture * sdlTexture = display->CreateSDLTextureFromImage(texture->filename);
    SDL_QueryTexture(
      sdlTexture,
      &(texture->format),
      &(texture->access),
      &(texture->width),
      &(texture->height)
    );

    texture->sdlTexture = sdlTexture;

    return 0;
  }

  Texture::Texture(std::string filename) : filename(filename) {
    sdlTexture = nullptr;
    loadingThread = nullptr;
  }

  void Texture::Load() {
    if (loadingThread != nullptr || sdlTexture != nullptr) {
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

  bool Texture::IsLoaded() {
    return sdlTexture != nullptr;
  }

  void Texture::Draw(SDLDisplay * display) {
    if (IsLoaded()) {
      display->DrawSDLTexture(sdlTexture, width, height);
    }
  }

}
