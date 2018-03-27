#include <v8/v8.h>
#include <v8/libplatform/libplatform.h>

#include "game.h"

#define FPS 60

namespace purr {
  Game * Game::instance = NULL;

  int Game::RunRenderingLoop(void * gameInstancePtr) {
    Game * game = static_cast<Game *>(gameInstancePtr);
    unsigned int currentUpdateTime = SDL_GetTicks();
    unsigned int lastUpdateTime = SDL_GetTicks();
    unsigned int currentDrawTime = SDL_GetTicks();
    unsigned int lastDrawTime = SDL_GetTicks();
    unsigned int fixedDeltaTime = 1000 / FPS;

    v8::Platform* platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform);
    v8::V8::Initialize();
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    game->isolate = v8::Isolate::New(create_params);

    {
      v8::Isolate::Scope isolate_scope(game->isolate);
      v8::HandleScope handle_scope(game->isolate);
      game->api = new API(game->isolate);
      game->console = new Console(game->isolate);
      game->main = game->SaveModule(game->mainFilename);

      while (game->eventLoopActivated) {
        currentUpdateTime = SDL_GetTicks();
        game->main->CallExportedFunction("update");
        currentDrawTime = SDL_GetTicks();

        if (currentDrawTime - lastDrawTime < fixedDeltaTime) {
          SDL_Delay(fixedDeltaTime - (currentDrawTime - lastDrawTime));
        }

        game->display->Clear();
        game->main->CallExportedFunction("draw");
        game->display->Render();
        lastDrawTime = currentDrawTime;
        lastUpdateTime = currentUpdateTime;
      }
    }

    delete create_params.array_buffer_allocator;
    return 0;
  }

  Game::Game(std::string mainFilename) : mainFilename(mainFilename) {}

  Game * Game::Instance() {
    return instance;
  }

  Game * Game::CreateInstance(std::string mainFilename) {
    if (instance == nullptr) {
      instance = new Game(mainFilename);
    }

    return instance;
  }

  Module * Game::GetModuleFromRoot(v8::Local<v8::Object> root) {
    std::string filename = Module::GetFilenameFromRoot(root);

    return modules[filename];
  }

  Module * Game::SaveModule(std::string filename) {
    if (modules.count(filename) == 0) {
      modules[filename] = new Module(isolate, filename);
      modules[filename]->Run();
    }

    return modules[filename];
  }

  void Game::FeedContextAPI(v8::Local<v8::Context> context) {
    v8::Local<v8::Object> object = context->Global();

    api->FeedObject("purr", object);
    console->FeedObject("console", object);
  }

  SDLDisplay * Game::Display() {
    return display;
  }

  void Game::DeleteInstance() {
    delete instance;
  }

  void Game::RunLoop() {
    void * gameInstancePtr = static_cast<void *>(this);
    unsigned int quitGameRequestTime = 0;

    display = new SDLDisplay();

    eventLoopActivated = true;
    SDL_Thread * renderingThread = SDL_CreateThread(
      Game::RunRenderingLoop,
      "rendering-loop",
      gameInstancePtr
    );

    SDL_Event event;

    while (eventLoopActivated) {
      if (quitGameRequestTime > 0 && (SDL_GetTicks() - quitGameRequestTime) > 2000 / FPS) {
        eventLoopActivated = false;
      }

      while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
          display->Hide();
          quitGameRequestTime = SDL_GetTicks();
        }
      }
    }

    SDL_WaitThread(renderingThread, NULL);
  }
}
