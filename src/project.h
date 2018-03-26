#ifndef PURR_PROJECT_H
#define PURR_PROJECT_H

#include <map>
#include <SDL2/SDL.h>
#include <v8/v8.h>
#include "api.h"
#include "display.h"
#include "console.h"
#include "module.h"

namespace purr {

  class Project {
    private:
      static Project * instance;

      v8::Isolate * isolate;
      API * api;
      Console * console;
      SDLDisplay * display;
      std::map<std::string, Module *> modules;

      Project(v8::Isolate *);

    protected:
      ~Project();

    public:
      static Project * Instance();
      static Project * CreateInstance();
      static void DeleteInstance();

      Module * GetModuleFromRoot(v8::Local<v8::Object>);
      Module * SaveModule(std::string);
      SDLDisplay * Display();
      void FeedContextAPI(v8::Local<v8::Context>);
  };

}

#endif
