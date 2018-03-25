#ifndef PURR_CONSOLE_H
#define PURR_CONSOLE_H

#include <v8/v8.h>

namespace purr {
  class Console {
    private:
      v8::Isolate * isolate;
      v8::Persistent<v8::Object> root;
      v8::Local<v8::Context> context;

    public:
      static void Log(const v8::FunctionCallbackInfo<v8::Value>&);

      void FeedObject(const char *, v8::Local<v8::Object>);

      Console(v8::Isolate*);
  };
}

#endif
