#include <iostream>
#include <string>
#include <fstream>
#include <filesystem/path.h>
#include <filesystem/resolver.h>

#include "module.h"
#include "project.h"

namespace purr {
  static std::string ValueToSTDString(v8::Local<v8::Value> value) {
    v8::Local<v8::String> castedValue = value->ToString();
    v8::String::Utf8Value utf8Value(castedValue);
    std::string strValue(*utf8Value);

    return strValue;
  }

  static void ExportsGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Persistent<v8::Value> * exports = Project::Instance()->GetModuleFromRoot(info.Holder())->GetExports();
    info.GetReturnValue().Set(*exports);
  }

  static void ExportsSetter(v8::Local<v8::String > property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info) {
    Project::Instance()->GetModuleFromRoot(info.Holder())->SetExports(value);
  }

  static void ModuleGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Object> module = info.Holder();
    info.GetReturnValue().Set(module);
  }

  static void ConsoleLog(const v8::FunctionCallbackInfo<v8::Value> &args) {
    for (unsigned index = 0; index < args.Length(); ++index) {
      if (index > 0) {
        std::cout << " ";
      }

      std::cout << ValueToSTDString(args[index]);
    }

    std::cout << std::endl;
  }

  Module::Module(
    v8::Isolate * isolate,
    std::string filename
  ) : isolate(isolate), filename(filename) {}

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

    v8::Local<v8::ObjectTemplate> consoleTemplate = v8::ObjectTemplate::New(isolate);
    consoleTemplate->Set(v8::String::NewFromUtf8(isolate, "log"), v8::FunctionTemplate::New(isolate, &ConsoleLog));

    v8::Local<v8::ObjectTemplate> moduleTemplate = v8::ObjectTemplate::New(isolate);

    moduleTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "exports"), &ExportsGetter, &ExportsSetter);
    moduleTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "module"), &ModuleGetter);

    v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, moduleTemplate);
    v8::Context::Scope context_scope(context);

    v8::Local<v8::Object> initialConsole = consoleTemplate->NewInstance();
    console = new v8::Persistent<v8::Object>(isolate, initialConsole);
    v8::Local<v8::Object> localConsole = v8::Local<v8::Object>::New(isolate, *console);

    v8::Local<v8::Object> initialExports = v8::Object::New(isolate);
    exports = new v8::Persistent<v8::Value>(isolate, initialExports);

    v8::Local<v8::String> filenameUTF8 = v8::String::NewFromUtf8(
      isolate,
      filename.c_str(),
      v8::NewStringType::kNormal
    ).ToLocalChecked();

    context->Global()->Set(v8::String::NewFromUtf8(isolate, "__filename__"), filenameUTF8);
    context->Global()->Set(v8::String::NewFromUtf8(isolate, "console"), localConsole);

    v8::Local<v8::String> source = v8::String::NewFromUtf8(
      isolate,
      scriptSource.c_str(),
      v8::NewStringType::kNormal
    ).ToLocalChecked();

    v8::Script::Compile(context, source).ToLocalChecked()->Run(context);
  }

  v8::Persistent<v8::Value> * Module::GetExports() {
    return exports;
  }

  void Module::SetExports(v8::Local<v8::Value> value) {
    exports->Reset(isolate, value);
  }

  std::string Module::GetFilenameFromRoot(v8::Local<v8::Object> root) {
    v8::Local<v8::Value> filename = root->Get(v8::String::NewFromUtf8(root->GetIsolate(), "__filename__"));

    return ValueToSTDString(filename);
  }

}
