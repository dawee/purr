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

  class Engine : public Worker, public Registry<Module> {
    private:
      v8::Isolate * isolate;
      Graphics * graphics;
      Console * console;
      SDLDisplay * display;
      std::map<std::string, Module *> modules;
      MainModule * main;
      std::string mainFilename;
      bool eventLoopActivated;
      Queue<Job> jobsQueue;
      Queue<Event> eventsQueue;

      static int runRenderingLoop(void *);
      static int runJobsLoop(void *);

    public:
      Module * Save(std::string);
      Module * FindRelative(v8::Local<v8::Context>, std::string, std::string);

      void RunLoop();
      void PushJob(Job *);

      Engine(std::string);
  };

}

#endif
