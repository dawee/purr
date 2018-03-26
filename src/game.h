#ifndef PURR_GAME_H
#define PURR_GAME_H

#include <map>
#include <SDL2/SDL.h>
#include <v8/v8.h>
#include "api.h"
#include "display.h"
#include "console.h"
#include "module.h"

namespace purr {

  class Game {
    private:
      static Game * instance;

      v8::Isolate * isolate;
      API * api;
      Console * console;
      SDLDisplay * display;
      std::map<std::string, Module *> modules;
      std::string mainFilename;

      Game(v8::Isolate *, std::string);

    protected:
      ~Game();

    public:
      static Game * Instance();
      static Game * CreateInstance(v8::Isolate *, std::string mainFilename);
      static void DeleteInstance();

      Module * GetModuleFromRoot(v8::Local<v8::Object>);
      Module * SaveModule(std::string);
      SDLDisplay * Display();
      void FeedContextAPI(v8::Local<v8::Context>);
      void RunLoop();
  };

}

#endif
