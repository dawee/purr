#ifndef PURR_MODULE_H
#define PURR_MODULE_H

#include <string>
#include <v8/v8.h>

#include "event.h"
#include "feeder.h"
#include "registry.h"

namespace purr {

  class Module {
    private:
      std::string filename;
      v8::Persistent<v8::Value> exports;
      v8::Persistent<v8::Object> console;
      v8::Persistent<v8::Object> root;
      Registry<Module> * registry;

      static void getExports(v8::Local<v8::Name>, const v8::PropertyCallbackInfo<v8::Value>&);
      static void setExports(v8::Local<v8::Name>, v8::Local<v8::Value>, const v8::PropertyCallbackInfo<void>&);
      static void getModule(v8::Local<v8::Name>, const v8::PropertyCallbackInfo<v8::Value>&);
      static void require(const v8::FunctionCallbackInfo<v8::Value>&);

      v8::Local<v8::Value> localExports();
      std::string dir();
      Module * getRelative(std::string);

    protected:
      v8::Isolate* isolate;
      v8::Local<v8::Context> context;

      v8::MaybeLocal<v8::Function> getExportedFunction(const char *);

    public:
      static std::string GetFilenameFromRoot(v8::Local<v8::Object>);

      void Run();
      void Feed(const char *, Feeder *);
      std::string ResolveRelativePath(const char *);

      Module(v8::Isolate *, std::string, Registry<Module> * reg);
  };

  class MainModule : public Module {
    public:
      void Dispatch(Event& event);
      void Draw();
      void Update(unsigned);
  };

}

#endif
