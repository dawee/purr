#include <iostream>

#include "console.h"
#include "util.h"

namespace purr {
  void Console::Log(const v8::FunctionCallbackInfo<v8::Value> &args) {
    for (unsigned index = 0; index < args.Length(); ++index) {
      if (index > 0) {
        std::cout << " ";
      }

      std::cout << ValueToSTDString(args[index]);
    }

    std::cout << std::endl;
  }

  Console::Console(v8::Isolate * isolate) : Feeder::Feeder(isolate) {
    v8::Local<v8::ObjectTemplate> consoleTemplate = v8::ObjectTemplate::New(isolate);
    consoleTemplate->Set(v8::String::NewFromUtf8(isolate, "log"), v8::FunctionTemplate::New(isolate, &Console::Log));

    v8::Context::Scope context_scope(context);

    v8::Local<v8::Object> localRoot = consoleTemplate->NewInstance();
    root.Reset(isolate, localRoot);
  }
}
