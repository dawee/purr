#include <iostream>
#include <string>
#include <fstream>
#include <filesystem/path.h>
#include <filesystem/resolver.h>

#include "module.h"
#include "engine.h"
#include "util.h"

namespace purr {
  static bool readFile(std::string filename, std::string& data) {
    std::ifstream file;

    file.open(filename);

    if (!file.is_open()) {
      return false;
    }

    file.seekg(0, std::ios::end);
    data.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    data.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    return true;
  }

  void Module::getExports(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Module * module = static_cast<Module *>(v8::Local<v8::External>::Cast(
      info.Data()->ToObject()->GetInternalField(0)
    )->Value());

    info.GetReturnValue().Set(module->localExports());
  }

  void Module::setExports(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info) {
    Module * module = static_cast<Module *>(v8::Local<v8::External>::Cast(
      info.Data()->ToObject()->GetInternalField(0)
    )->Value());

    module->exports.Reset(module->isolate, value);
  }

  void Module::getModule(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Module * module = static_cast<Module *>(v8::Local<v8::External>::Cast(
      info.Data()->ToObject()->GetInternalField(0)
    )->Value());

    info.GetReturnValue().Set(v8::Local<v8::Object>::New(module->isolate, module->root));
  }

  void Module::require(const v8::FunctionCallbackInfo<v8::Value>& info) {
    Module * module = static_cast<Module *>(v8::Local<v8::External>::Cast(
      info.Data()->ToObject()->GetInternalField(0)
    )->Value());

    if (info.Length() == 0) {
      std::cerr << "Error: require() called without any arguments";
      return;
    }

    if (!info[0]->IsString()) {
      std::cerr << "Error: require() called with an invalid type (string expected)";
      return;
    }

    std::string relativePath = ValueToSTDString(info[0]);
    Module * foundModule = module->registry->FindRelative(relativePath, module->dir());

    if (foundModule != nullptr) {
      info.GetReturnValue().Set(foundModule->localExports());
    }
  }

  Module::Module(
    v8::Isolate * isolate,
    std::string filename,
    Registry<Module> * registry
  ) : isolate(isolate), filename(filename), registry(registry) {
    v8::Local<v8::ObjectTemplate> moduleTemplate = v8::ObjectTemplate::New(isolate);
    moduleTemplate->SetInternalFieldCount(1);

    context = v8::Context::New(isolate, NULL, moduleTemplate);
    v8::Context::Scope context_scope(context);
    context->Global()->SetInternalField(0, v8::External::New(isolate, this));

    if (
      context->Global()->SetAccessor(
        context,
        v8::String::NewFromUtf8(isolate, "exports"),
        getExports,
        setExports,
        context->Global()
      ).ToChecked() &&
      context->Global()->SetAccessor(
        context,
        v8::String::NewFromUtf8(isolate, "module"),
        getModule,
        0,
        context->Global()
      ).ToChecked() &&
      context->Global()->Set(
        context,
        v8::String::NewFromUtf8(isolate, "require"),
        v8::Function::New(context, require, context->Global()).ToLocalChecked()
      ).ToChecked()
    ) {
      v8::Local<v8::String> filenameUTF8 = v8::String::NewFromUtf8(isolate, filename.c_str());
      v8::Local<v8::String> dirnameUTF8 = v8::String::NewFromUtf8(isolate,dir().c_str());
      v8::Local<v8::Object> localExports = v8::Object::New(isolate);

      exports.Reset(isolate, localExports);
      root.Reset(isolate, context->Global());
      context->Global()->Set(v8::String::NewFromUtf8(isolate, "__filename__"), filenameUTF8);
      context->Global()->Set(v8::String::NewFromUtf8(isolate, "__dirname__"), dirnameUTF8);
    }
  }

  void Module::Run() {
    std::string scriptSource;

    if (!readFile(filename, scriptSource)) {
      std::cerr << "could not find module " << filename << std::endl;
      return;
    }

    v8::Context::Scope context_scope(context);

    v8::Local<v8::String> source = v8::String::NewFromUtf8(
      isolate,
      scriptSource.c_str(),
      v8::NewStringType::kNormal
    ).ToLocalChecked();

    v8::Script::Compile(context, source).ToLocalChecked()->Run(context).ToLocalChecked();
  }

  v8::MaybeLocal<v8::Function> Module::getExportedFunction(const char * name) {
    v8::Local<v8::Value> localExports = v8::Local<v8::Value>::New(isolate, exports);
    v8::MaybeLocal<v8::Function> none;

    if (!localExports->IsObject()) {
      return none;
    }

    v8::Local<v8::Object> exportsObject = localExports->ToObject();
    v8::Local<v8::String> key = v8::String::NewFromUtf8(isolate, name);
    v8::MaybeLocal<v8::Value> maybeField = exportsObject->Get(key);

    if (maybeField.IsEmpty() || !(maybeField.ToLocalChecked()->IsFunction())) {
      return none;
    }

    v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(maybeField.ToLocalChecked());
    v8::MaybeLocal<v8::Function> maybe(func);

    return maybe;
  }

  void Module::Feed(const char * key, Feeder * feeder) {
    v8::Context::Scope context_scope(context);

    feeder->FeedObject(key, v8::Local<v8::Object>::New(isolate, root));
  }

  v8::Local<v8::Value> Module::localExports() {
    return v8::Local<v8::Value>::New(isolate, exports);
  }

  std::string Module::GetFilenameFromRoot(v8::Local<v8::Object> root) {
    v8::Local<v8::Value> filename = root->Get(v8::String::NewFromUtf8(root->GetIsolate(), "__filename__"));

    return ValueToSTDString(filename);
  }

  std::string Module::dir() {
    filesystem::path filePath(filename);
    return filePath.parent_path().str();
  }

  std::string Module::ResolveRelativePath(const char * relativePathStr) {
    filesystem::path dirPath(dir());
    filesystem::path relativePath(relativePathStr);

    return (dirPath / relativePath).make_absolute().str();
  }

  void MainModule::Dispatch(Event& event) {
    v8::Context::Scope context_scope(context);
    v8::MaybeLocal<v8::Function> maybeFunc = getExportedFunction("dispatch");

    if (!maybeFunc.IsEmpty()) {
      v8::Handle<v8::Value> args[1];

      args[0] = event.ToJS(isolate, context);
      v8::Local<v8::Function> func = maybeFunc.ToLocalChecked();
      func->Call(func, 1, args);
    }
  }

  void MainModule::Draw() {
    v8::Context::Scope context_scope(context);
    v8::MaybeLocal<v8::Function> maybeFunc = getExportedFunction("draw");

    if (!maybeFunc.IsEmpty()) {
      v8::Local<v8::Function> func = maybeFunc.ToLocalChecked();
      func->Call(func, 0, NULL);
    }
  }

  void MainModule::Update(unsigned dt) {
    v8::Context::Scope context_scope(context);
    v8::MaybeLocal<v8::Function> maybeFunc = getExportedFunction("update");

    if (!maybeFunc.IsEmpty()) {
      v8::Handle<v8::Value> args[1];

      args[0] = v8::Number::New(isolate, dt);
      v8::Local<v8::Function> func = maybeFunc.ToLocalChecked();
      func->Call(func, 1, args);
    }
  }
}
