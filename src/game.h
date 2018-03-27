#ifndef PURR_GAME_H
#define PURR_GAME_H

#include <map>
#include <SDL2/SDL.h>
#include <v8/v8.h>
#include "api.h"
#include "display.h"
#include "console.h"
#include "job.h"
#include "module.h"
#include "queue.h"

namespace purr {

  class Game {
    private:
      static Game * instance;

      v8::Isolate * isolate;
      API * api;
      Console * console;
      SDLDisplay * display;
      std::map<std::string, Module *> modules;
      Module * main;
      std::string mainFilename;
      bool eventLoopActivated;
      Queue<Job> jobsQueue;

      Game(std::string);

    public:
      static Game * Instance();
      static Game * CreateInstance(std::string mainFilename);
      static void DeleteInstance();
      static int RunRenderingLoop(void *);
      static int RunJobsLoop(void *);

      Module * GetModuleFromRoot(v8::Local<v8::Object>);
      Module * SaveModule(std::string);
      SDLDisplay * Display();
      void FeedContextAPI(v8::Local<v8::Context>);
      void RunLoop();
      void PushJob(Job *);
  };

}

#endif
