#include <v8/v8.h>
#include <v8/libplatform/libplatform.h>

#include "queue.h"
#include "game.h"

#define FPS 60

namespace purr {
  int Game::runRenderingLoop(void * gameInstancePtr) {
    Game * game = static_cast<Game *>(gameInstancePtr);
    unsigned int currentUpdateTime = SDL_GetTicks();
    unsigned int lastUpdateTime = SDL_GetTicks();
    unsigned int fixedDeltaTime = 1000 / FPS;

    SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);

    v8::Platform* platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform);
    v8::V8::Initialize();
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    game->isolate = v8::Isolate::New(create_params);

    {
      v8::Isolate::Scope isolate_scope(game->isolate);
      v8::HandleScope handle_scope(game->isolate);

      game->api = new API(game->isolate, game->display, static_cast<Worker *>(game));
      game->console = new Console(game->isolate);
      game->main = game->Save(game->mainFilename);

      while (game->eventLoopActivated) {
        currentUpdateTime = SDL_GetTicks();

        if (currentUpdateTime - lastUpdateTime < fixedDeltaTime) {
          SDL_Delay(fixedDeltaTime - (currentUpdateTime - lastUpdateTime));
          currentUpdateTime = SDL_GetTicks();
        }

        game->main->CallExportedFunction("update", currentUpdateTime - lastUpdateTime);
        game->display->Clear();
        game->main->CallExportedFunction("draw");
        game->display->Render();
        lastUpdateTime = currentUpdateTime;
      }
    }

    delete create_params.array_buffer_allocator;
    return 0;
  }

  int Game::runJobsLoop(void * gameInstancePtr) {
    Game * game = static_cast<Game *>(gameInstancePtr);

    SDL_SetThreadPriority(SDL_THREAD_PRIORITY_LOW);

    while (game->eventLoopActivated) {
      Job * job = game->jobsQueue.Pull();

      if (job != nullptr) {
        job->Run();
        delete job;
      } else {
        SDL_Delay(2000 / FPS);
      }
    }

    return 0;
  }

  Game::Game(std::string mainFilename) : mainFilename(mainFilename) {}

  Module * Game::GetFromHolder(v8::Local<v8::Object> root) {
    std::string filename = Module::GetFilenameFromRoot(root);

    return modules[filename];
  }

  Module * Game::Save(std::string filename) {
    if (modules.count(filename) == 0) {
      modules[filename] = new Module(isolate, filename, static_cast<Registry<Module> *>(this));

      modules[filename]->Feed("purr", static_cast<Feeder *>(api));
      modules[filename]->Feed("console", static_cast<Feeder *>(console));
      modules[filename]->Run();
    }

    return modules[filename];
  }

  void Game::RunLoop() {
    void * gameInstancePtr = static_cast<void *>(this);
    unsigned int quitGameRequestTime = 0;

    display = new SDLDisplay();

    eventLoopActivated = true;

    SDL_Thread * renderingThread = SDL_CreateThread(
      Game::runRenderingLoop,
      "rendering-loop",
      gameInstancePtr
    );

    SDL_Thread * jobsThread = SDL_CreateThread(
      Game::runJobsLoop,
      "jobs-loop",
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

    jobsQueue.Push(new NoopJob());
    SDL_WaitThread(jobsThread, NULL);
    SDL_WaitThread(renderingThread, NULL);
  }

  void Game::PushJob(Job * job) {
    jobsQueue.Push(job);
  }
}
