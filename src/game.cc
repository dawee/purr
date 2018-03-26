#include "game.h"

namespace purr {
  Game * Game::instance = NULL;

  Game::Game(v8::Isolate * isolate, std::string mainFilename) : isolate(isolate), mainFilename(mainFilename) {
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
      instance = new Game(isolate, mainFilename);
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
    purr::Module * main = SaveModule(mainFilename);
    SDL_Event event;
    bool quit = false;

    while (!quit) {
      while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
          display->Hide();
          quit = true;
        }
      }

      main->CallExportedFunction("update");
      display->Clear();
      main->CallExportedFunction("draw");
      display->Render();
    }
  }
}
