#include <iostream>

#include "game.h"
#include "texture.h"

namespace purr {
  int Texture::LoadTexture(void * texturePtr) {
    Texture * texture = static_cast<Texture *>(texturePtr);

    return 0;
  }

  Texture::Texture(std::string filename) : filename(filename) {
    sdlTexture = nullptr;
    loadingThread = nullptr;
    mutex = SDL_CreateMutex();
  }

  void Texture::setSDLTexture(SDL_Texture * sdlTexture) {
    if (SDL_LockMutex(mutex) == 0) {
      SDL_QueryTexture(sdlTexture, &format, &access, &width, &height);

      this->sdlTexture = sdlTexture;

      SDL_UnlockMutex(mutex);
    }
  }

  void Texture::Load() {
    if (sdlTexture != nullptr) {
      return;
    }

    SDLDisplay * display = Game::Instance()->Display();
    SDL_Texture * sdlTexture = display->CreateSDLTextureFromImage(filename);

    setSDLTexture(sdlTexture);
  }

  bool Texture::IsLoaded() {
    return sdlTexture != nullptr;
  }

  void Texture::Draw(SDLDisplay * display, int x, int y) {
    if (SDL_LockMutex(mutex) == 0) {
      if (IsLoaded()) {
        display->DrawSDLTexture(sdlTexture, x, y, width, height);
      }

      SDL_UnlockMutex(mutex);
    }
  }

}
