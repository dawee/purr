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

  Texture::~Texture() {
    if (sdlTexture != nullptr) {
      SDL_DestroyTexture(sdlTexture);
    }
  }

  void Texture::Load() {
    if (SDL_LockMutex(mutex) == 0) {
      if (sdlTexture == nullptr) {
        SDLDisplay * display = Game::Instance()->Display();
        SDL_Texture * sdlTexture = display->CreateSDLTextureFromImage(filename);
        SDL_QueryTexture(sdlTexture, &format, &access, &width, &height);
        this->sdlTexture = sdlTexture;
      }

      SDL_UnlockMutex(mutex);
    }
  }

  bool Texture::IsLoaded() {
    bool isLoaded = false;

    if (SDL_LockMutex(mutex) == 0) {
      isLoaded = (sdlTexture != nullptr);

      SDL_UnlockMutex(mutex);
    }

    return isLoaded;
  }

  void Texture::Draw(SDLDisplay * display, int x, int y) {
    if (SDL_LockMutex(mutex) == 0) {
      if (IsLoaded()) {
        display->DrawSDLTexture(sdlTexture, x, y, width, height);
      }

      SDL_UnlockMutex(mutex);
    }
  }

  int Texture::Width() {
    int width = 0;

    if (SDL_LockMutex(mutex) == 0) {
      width = this->width;
      SDL_UnlockMutex(mutex);
    }

    return width;
  }

  int Texture::Height()  {
    int height = 0;

    if (SDL_LockMutex(mutex) == 0) {
      height = this->height;
      SDL_UnlockMutex(mutex);
    }

    return height;
  }
}
