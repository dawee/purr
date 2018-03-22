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
      v8::Local<v8::ObjectTemplate> moduleTemplate;
      std::map<std::string, Module *> modules;
      Project(v8::Isolate *);

    public:
      static Project * Instance();
      static void ExportsGetter(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>&);
      static void ExportsSetter(v8::Local<v8::String>, v8::Local<v8::Value>, const v8::PropertyCallbackInfo<void> &);
      static void ModuleGetter(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>&);
      Module * GetModuleFromRoot(v8::Local<v8::Object>);
      Module * Require(std::string);
  };

}

#endif
