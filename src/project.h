#ifndef PURR_PROJECT_H
#define PURR_PROJECT_H

#include <map>
#include <v8/v8.h>
#include "module.h"

namespace purr {

  class Project {
    private:
      static Project * instance;

      v8::Isolate * isolate;
      std::map<std::string, Module *> modules;

      Project(v8::Isolate *);

    public:
      static Project * Instance();

      Module * GetModuleFromRoot(v8::Local<v8::Object>);
      Module * Require(std::string);
  };

}

#endif
