#include <v8/v8.h>
#include <v8/libplatform/libplatform.h>
#include <filesystem/path.h>
#include <filesystem/resolver.h>

#include "event.h"
#include "queue.h"
#include "engine.h"

static int const FPS = 60;
static int const FRAME_DURATION = 1000 / FPS;

namespace purr {
  static bool readFile(std::string filename, std::string& data) {
    std::ifstream file;

    file.open(filename);

    if (!file.is_open()) {
      return false;
    }

    file.seekg(0, std::ios::end);
    data.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    data.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    return true;
  }

  int Engine::runRenderingLoop(void * engineInstancePtr) {
    Engine * engine = static_cast<Engine *>(engineInstancePtr);
    unsigned int currentUpdateTime = SDL_GetTicks();
    unsigned int lastUpdateTime = SDL_GetTicks();

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

      /* Bindings */
      engine->graphics = new Graphics(engine->isolate, engine->display, static_cast<Worker *>(engine));
      engine->console = new Console(engine->isolate);

      /* Natives */
      engine->nativeConsoleFeeder = new NativeFeeder(engine, "console");

      /* Scripts */

      MainModule * entry = static_cast<MainModule *>(engine->Resolve("_entry", engine->currentDir));

      engine->main = static_cast<MainModule *>(engine->Resolve(engine->mainFilename, engine->currentDir));

      while (engine->eventLoopActivated) {
        Event * event;

        do {
          event = engine->eventsQueue.PullWithoutWaiting();

          if (event != nullptr) {
            entry->Dispatch(*event);
            delete event;
          }
        } while (event != nullptr);

        currentUpdateTime = SDL_GetTicks();

        if (currentUpdateTime - lastUpdateTime < FRAME_DURATION) {
          SDL_Delay(FRAME_DURATION - (currentUpdateTime - lastUpdateTime));
          currentUpdateTime = SDL_GetTicks();
        }

        entry->Update(currentUpdateTime - lastUpdateTime);
        engine->display->Clear();
        entry->Draw();
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
        SDL_Delay(2 * FRAME_DURATION);
      }
    }

    return 0;
  }

  Engine::Engine(
    std::string mainFilename,
    std::string currentDir
  ) : mainFilename(mainFilename), currentDir(currentDir) {
    main = nullptr;
  }

  Module * Engine::findAbsolute(std::string filename, bool feedWithNatives) {
    if (modules.count(filename) == 0) {
      modules[filename] = new Module(isolate, filename, this);

      modules[filename]->Feed("_graphics", graphics);
      modules[filename]->Feed("_console", console);

      if (feedWithNatives) {
        modules[filename]->Feed("console", nativeConsoleFeeder);
      }

      modules[filename]->Run();
    }

    return modules[filename];
  }

  Module * Engine::findRelative(std::string filename, std::string dirname, bool feedWithNatives) {
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    v8::Context::Scope context_scope(context);

    filesystem::path dirPath(dirname);
    filesystem::path fullPath(dirPath / filename);
    filesystem::path packageName("package.json");
    filesystem::path fullPathPackageJSON(fullPath / packageName);
    std::string packageData;

    if (fullPathPackageJSON.is_file() && readFile(fullPathPackageJSON.make_absolute().str(), packageData)) {
      v8::MaybeLocal<v8::Value> maybePackage = v8::JSON::Parse(
        context,
        v8::String::NewFromUtf8(isolate, packageData.c_str())
      );

      if (maybePackage.IsEmpty()) {
        std::cerr << "Failed to parse " << fullPathPackageJSON.make_absolute().str() << std::endl;
        return nullptr;
      }

      v8::Local<v8::Value> package = maybePackage.ToLocalChecked();

      if (!package->IsObject()) {
        std::cerr << "Bad package format : " << fullPathPackageJSON.make_absolute().str() << std::endl;
        return nullptr;
      }

      std::string mainScriptName;
      v8::MaybeLocal<v8::Value> maybeMainScriptValue = package->ToObject()->Get(
        context,
        v8::String::NewFromUtf8(isolate, "main")
      );

      if (maybeMainScriptValue.IsEmpty()) {
        mainScriptName = "index.js";
      } else if (!maybeMainScriptValue.ToLocalChecked()->IsString()) {
        std::cerr << "Bad main value found in : " << fullPathPackageJSON.make_absolute().str() << std::endl;
        return nullptr;
      } else {
        v8::String::Utf8Value utf8MainName(
          maybeMainScriptValue.ToLocalChecked()->ToString(context).ToLocalChecked()
        );

        mainScriptName = std::string(*utf8MainName);
      }

      filesystem::path mainRelativePath(mainScriptName);
      filesystem::path fullPackageMainPath(fullPath / mainRelativePath);

      if (fullPackageMainPath.is_file()) {
        return findAbsolute(fullPackageMainPath.make_absolute().str(), feedWithNatives);
      }
    }

    if (fullPath.is_file()) {
      return findAbsolute(fullPath.make_absolute().str(), feedWithNatives);
    }

    filesystem::path fullPathJS(fullPath.str() + ".js");

    if (fullPathJS.is_file()) {
      return findAbsolute(fullPathJS.make_absolute().str(), feedWithNatives);
    }

    filesystem::path indexName("index.js");
    filesystem::path fullPathIndexJS(fullPath / indexName);

    if (fullPathIndexJS.is_file()) {
      return findAbsolute(fullPathIndexJS.make_absolute().str(), feedWithNatives);
    }

    return nullptr;
  }

  Module * Engine::resolve(std::string query, std::string dirname, bool feedWithNatives) {
    filesystem::path queryPath(query);

    if (queryPath.filename() == query) {
      Module * module = nullptr;
      filesystem::path binPath(SDL_GetBasePath());
      filesystem::path nativeLibsPath(binPath.parent_path() / "lib" / "purr");

      module = findRelative(query, nativeLibsPath.str(), feedWithNatives);

      if (module != nullptr) {
        return module;
      }

      filesystem::path currentPath(currentDir);
      filesystem::path nodeModulesPath(currentPath / "node_modules");

      return findRelative(query, nodeModulesPath.str(), feedWithNatives);
    }

    return findRelative(query, dirname, feedWithNatives);
  }

  Module * Engine::Resolve(std::string query, std::string dirname) {
    return resolve(query, dirname, true);
  }

  int Engine::RunLoop() {
    void * engineInstancePtr = static_cast<void *>(this);
    unsigned int quitEngineRequestTime = 0;

    display = new SDLDisplay();
    eventLoopActivated = true;

    for (int joystickIndex = 0; joystickIndex < SDL_NumJoysticks(); ++joystickIndex) {
      if (SDL_IsGameController(joystickIndex)) {
        // @TODO manage controllers pointers
        /* SDL_GameController * controller = */SDL_GameControllerOpen(joystickIndex);
      }
    }

    SDL_Thread * renderingThread = SDL_CreateThread(runRenderingLoop, "rendering-loop", engineInstancePtr);
    SDL_Thread * jobsThread = SDL_CreateThread(runJobsLoop, "jobs-loop", engineInstancePtr);
    SDL_Event sdlEvent;

    while (eventLoopActivated) {
      if (quitEngineRequestTime > 0 && (SDL_GetTicks() - quitEngineRequestTime) > (2 * FRAME_DURATION)) {
        eventLoopActivated = false;
      }

      while (SDL_PollEvent(&sdlEvent) != 0) {
        Event * event = Event::FromSDLEvent(sdlEvent);

        if (event != nullptr) {
          eventsQueue.Push(event);
        }

        if (sdlEvent.type == SDL_QUIT || (sdlEvent.type == SDL_WINDOWEVENT && sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE)) {
          display->Hide();
          quitEngineRequestTime = SDL_GetTicks();
        }
      }
    }

    jobsQueue.Push(new NoopJob());
    SDL_WaitThread(jobsThread, NULL);
    SDL_WaitThread(renderingThread, NULL);

    return 0;
  }

  void Engine::PushJob(Job * job) {
    jobsQueue.Push(job);
  }

  NativeFeeder::NativeFeeder(Engine * engine, const char * name) : Feeder(engine->isolate) {
    Module * nativeModule = engine->resolve(name, engine->currentDir, false);

    if (nativeModule != nullptr) {
      v8::Context::Scope context_scope(context);

      root.Reset(isolate, nativeModule->Exports());
    }
  }
}
