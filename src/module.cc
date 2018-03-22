#include <iostream>
#include <string>
#include <fstream>
#include <filesystem/path.h>
#include <filesystem/resolver.h>

#include "module.h"

namespace purr {
  Module::Module(
    v8::Isolate * isolate,
    v8::Local<v8::ObjectTemplate> moduleTemplate,
    std::string filename
  ) : isolate(isolate), moduleTemplate(moduleTemplate), filename(filename) {
  }

  void Module::Run() {
    std::ifstream scriptFile;

    scriptFile.open(filename);

    if (!scriptFile.is_open()) {
      std::cerr << "failed to read" << filename << std::endl;
    }

    std::string scriptSource;

    scriptFile.seekg(0, std::ios::end);
    scriptSource.reserve(scriptFile.tellg());
    scriptFile.seekg(0, std::ios::beg);
    scriptSource.assign((std::istreambuf_iterator<char>(scriptFile)), std::istreambuf_iterator<char>());
    scriptFile.close();

    v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, moduleTemplate);
    v8::Context::Scope context_scope(context);
    v8::Local<v8::Object> initialExports = v8::Object::New(isolate);
    exports = new v8::Persistent<v8::Value>(isolate, initialExports);

    v8::Local<v8::String> filenameUTF8 = v8::String::NewFromUtf8(
      isolate,
      filename.c_str(),
      v8::NewStringType::kNormal
    ).ToLocalChecked();

    context->Global()->Set(v8::String::NewFromUtf8(isolate, "__filename__"), filenameUTF8);

    v8::Local<v8::String> source = v8::String::NewFromUtf8(
      isolate,
      scriptSource.c_str(),
      v8::NewStringType::kNormal
    ).ToLocalChecked();

    v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();

    v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
    v8::String::Utf8Value utf8(isolate, result);

    std::cout << *utf8 << std::endl;
  }

  v8::Persistent<v8::Value> * Module::GetExports() {
    return exports;
  }

  void Module::SetExports(v8::Local<v8::Value> value) {
    exports->Reset(isolate, value);
  }

  std::string Module::GetFilenameFromRoot(v8::Local<v8::Object> root) {
    v8::Local<v8::Value> value = root->Get(v8::String::NewFromUtf8(root->GetIsolate(), "__filename__"));
    v8::Local<v8::String> castedValue = value->ToString();
    v8::String::Utf8Value utf8Value(castedValue);
    std::string strValue(*utf8Value);

    return strValue;
  }

}
