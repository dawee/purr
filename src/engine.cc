#include <v8/v8.h>
#include <v8/libplatform/libplatform.h>

#include "queue.h"
#include "engine.h"

#define FPS 60

namespace purr {
  int Engine::runRenderingLoop(void * engineInstancePtr) {
    Engine * engine = static_cast<Engine *>(engineInstancePtr);
    unsigned int currentUpdateTime = SDL_GetTicks();
    unsigned int lastUpdateTime = SDL_GetTicks();
    unsigned int fixedDeltaTime = 1000 / FPS;

    SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);

    v8::Platform* platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform);
    v8::V8::Initialize();
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    engine->isolate = v8::Isolate::New(create_params);

    {
      v8::Isolate::Scope isolate_scope(engine->isolate);
      v8::HandleScope handle_scope(engine->isolate);

      engine->graphics = new Graphics(engine->isolate, engine->display, static_cast<Worker *>(engine));
      engine->console = new Console(engine->isolate);
      engine->main = engine->Save(engine->mainFilename);

      while (engine->eventLoopActivated) {
        currentUpdateTime = SDL_GetTicks();

        if (currentUpdateTime - lastUpdateTime < fixedDeltaTime) {
          SDL_Delay(fixedDeltaTime - (currentUpdateTime - lastUpdateTime));
          currentUpdateTime = SDL_GetTicks();
        }

        engine->main->CallExportedFunction("update", currentUpdateTime - lastUpdateTime);
        engine->display->Clear();
        engine->main->CallExportedFunction("draw");
        engine->display->Render();
        lastUpdateTime = currentUpdateTime;
      }
    }

    delete create_params.array_buffer_allocator;
    return 0;
  }

  int Engine::runJobsLoop(void * engineInstancePtr) {
    Engine * engine = static_cast<Engine *>(engineInstancePtr);

    SDL_SetThreadPriority(SDL_THREAD_PRIORITY_LOW);

    while (engine->eventLoopActivated) {
      Job * job = engine->jobsQueue.Pull();

      if (job != nullptr) {
        job->Run();
        delete job;
      } else {
        SDL_Delay(2000 / FPS);
      }
    }

    return 0;
  }

  Engine::Engine(std::string mainFilename) : mainFilename(mainFilename) {}

  Module * Engine::GetFromHolder(v8::Local<v8::Object> root) {
    std::string filename = Module::GetFilenameFromRoot(root);

    return modules[filename];
  }

  Module * Engine::Save(std::string filename) {
    if (modules.count(filename) == 0) {
      modules[filename] = new Module(isolate, filename, static_cast<Registry<Module> *>(this));

      modules[filename]->Feed("graphics", static_cast<Feeder *>(graphics));
      modules[filename]->Feed("console", static_cast<Feeder *>(console));
      modules[filename]->Run();
    }

    return modules[filename];
  }

  void Engine::RunLoop() {
    void * engineInstancePtr = static_cast<void *>(this);
    unsigned int quitEngineRequestTime = 0;

    display = new SDLDisplay();
    eventLoopActivated = true;

    SDL_Thread * renderingThread = SDL_CreateThread(runRenderingLoop, "rendering-loop", engineInstancePtr);
    SDL_Thread * jobsThread = SDL_CreateThread(runJobsLoop, "jobs-loop", engineInstancePtr);
    SDL_Event event;

    while (eventLoopActivated) {
      if (quitEngineRequestTime > 0 && (SDL_GetTicks() - quitEngineRequestTime) > 2000 / FPS) {
        eventLoopActivated = false;
      }

      while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
          display->Hide();
          quitEngineRequestTime = SDL_GetTicks();
        }
      }
    }

    jobsQueue.Push(new NoopJob());
    SDL_WaitThread(jobsThread, NULL);
    SDL_WaitThread(renderingThread, NULL);
  }

  void Engine::PushJob(Job * job) {
    jobsQueue.Push(job);
  }
}
