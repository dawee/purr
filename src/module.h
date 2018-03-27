#ifndef PURR_MODULE_H
#define PURR_MODULE_H

#include <string>
#include <v8/v8.h>

namespace purr {

  class Module {
    private:
      std::string filename;
      v8::Isolate* isolate;
      v8::Persistent<v8::Value> exports;
      v8::Persistent<v8::Object> console;
      v8::Local<v8::Context> context;

      v8::Local<v8::Value> GetExports();
      std::string GetDir();
      v8::Local<v8::Function> getExportedFunction(const char *);

    public:
      static std::string GetFilenameFromRoot(v8::Local<v8::Object>);
      static void ExportsGetter(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>&);
      static void ExportsSetter(v8::Local<v8::String>, v8::Local<v8::Value>, const v8::PropertyCallbackInfo<void>&);
      static void ModuleGetter(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>&);

      void Run();
      void CallExportedFunction(const char *);
      void CallExportedFunction(const char *, unsigned);
      std::string ResolveRelativePath(const char *);

      Module(v8::Isolate *, std::string);
  };

}

#endif
