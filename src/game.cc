#include "game.h"

#define FPS 60

namespace purr {
  Game * Game::instance = NULL;

  int Game::RunEventLoop(void * gameInstancePtr) {
    Game * game = static_cast<Game *>(gameInstancePtr);
    SDL_Event event;

    while (game->eventLoopActivated) {
      while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
          game->display->Hide();
          game->eventLoopActivated = false;
        }
      }
    }

    return 0;
  }

  Game::Game(v8::Isolate * isolate) : isolate(isolate) {
    eventLoopActivated = false;
    api = new API(isolate);
    console = new Console(isolate);
    display = new SDLDisplay();
  }

  Game::~Game() {
    delete api;
    delete console;
    delete display;
  }

  Game * Game::Instance() {
    return instance;
  }

  Game * Game::CreateInstance(v8::Isolate * isolate, std::string mainFilename) {
    if (instance == nullptr) {
      instance = new Game(isolate);
      instance->main = instance->SaveModule(mainFilename);
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
    unsigned int currentUpdateTime = SDL_GetTicks();
    unsigned int lastUpdateTime = SDL_GetTicks();
    unsigned int currentDrawTime = SDL_GetTicks();
    unsigned int lastDrawTime = SDL_GetTicks();
    unsigned int fixedDeltaTime = 1000 / FPS;

    eventLoopActivated = true;
    SDL_Thread * eventLoopThread = SDL_CreateThread(
      Game::RunEventLoop,
      "event-loop",
      gameInstancePtr
    );

    while (eventLoopActivated) {
      currentUpdateTime = SDL_GetTicks();
      main->CallExportedFunction("update");
      currentDrawTime = SDL_GetTicks();

      if (currentDrawTime - lastDrawTime < fixedDeltaTime) {
        SDL_Delay(fixedDeltaTime - (currentDrawTime - lastDrawTime));
      }

      display->Clear();
      main->CallExportedFunction("draw");
      display->Render();
      lastDrawTime = currentDrawTime;
      lastUpdateTime = currentUpdateTime;
    }

    SDL_WaitThread(eventLoopThread, NULL);
  }
}
