#ifndef PURR_GAME_H
#define PURR_GAME_H

#include <map>
#include <SDL2/SDL.h>
#include <v8/v8.h>
#include "graphics.h"
#include "display.h"
#include "console.h"
#include "job.h"
#include "module.h"
#include "queue.h"
#include "registry.h"
#include "worker.h"

namespace purr {

  class NativeFeeder;

  class Engine : public Worker, public Registry<Module> {
    private:
      v8::Isolate * isolate;
      Graphics * graphics;
      Console * console;
      SDLDisplay * display;
      std::map<std::string, Module *> modules;
      MainModule * main;
      std::string mainFilename;
      std::string currentDir;
      bool eventLoopActivated;
      Queue<Job> jobsQueue;
      Queue<Event> eventsQueue;
      NativeFeeder * nativeConsoleFeeder;
      v8::Persistent<v8::Value> exception;
      bool raisedException;

      static int runRenderingLoop(void *);
      static int runJobsLoop(void *);

      Module * resolve(std::string, std::string, bool);
      Module * findAbsolute(std::string, bool);
      Module * findRelative(std::string, std::string, bool);

    public:
      Module * Resolve(std::string, std::string);

      int RunLoop();
      void PushJob(Job *);

      Engine(std::string, std::string);

      friend class NativeFeeder;
  };

  class NativeFeeder : public Feeder {
    public:
      NativeFeeder(Engine *, const char *);
  };

}

#endif
