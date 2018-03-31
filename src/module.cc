#include <iostream>
#include <string>
#include <fstream>
#include <filesystem/path.h>
#include <filesystem/resolver.h>

#include "module.h"
#include "engine.h"
#include "util.h"

namespace purr {
  static void noop(const v8::FunctionCallbackInfo<v8::Value>&) {}

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
    filesystem::path dirPath(module->dir());
    filesystem::path fullPath(dirPath / relativePath);

    if (fullPath.is_file()) {
      Module * requiredModule = module->registry->Save(fullPath.make_absolute().str());
      info.GetReturnValue().Set(requiredModule->localExports());
      return;
    }

    filesystem::path fullPathJS(fullPath.str() + ".js");

    if (fullPathJS.is_file()) {
      Module * requiredModule = module->registry->Save(fullPathJS.make_absolute().str());
      info.GetReturnValue().Set(requiredModule->localExports());
      return;
    }

    filesystem::path indexName("index.js");
    filesystem::path fullPathIndexJS(fullPath / indexName);

    if (fullPathIndexJS.is_file()) {
      Module * requiredModule = module->registry->Save(fullPathIndexJS.make_absolute().str());
      info.GetReturnValue().Set(requiredModule->localExports());
      return;
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
    std::ifstream scriptFile;

    scriptFile.open(filename);

    if (!scriptFile.is_open()) {
      std::cerr << "could not find module " << filename << std::endl;
      return;
    }

    std::string scriptSource;

    scriptFile.seekg(0, std::ios::end);
    scriptSource.reserve(scriptFile.tellg());
    scriptFile.seekg(0, std::ios::beg);
    scriptSource.assign((std::istreambuf_iterator<char>(scriptFile)), std::istreambuf_iterator<char>());
    scriptFile.close();

    v8::Context::Scope context_scope(context);

    v8::Local<v8::String> source = v8::String::NewFromUtf8(
      isolate,
      scriptSource.c_str(),
      v8::NewStringType::kNormal
    ).ToLocalChecked();

    v8::Script::Compile(context, source).ToLocalChecked()->Run(context).ToLocalChecked();
  }

  v8::Local<v8::Function> Module::getExportedFunction(const char * name) {
    v8::Local<v8::Value> localExports = v8::Local<v8::Value>::New(isolate, exports);

    if (!localExports->IsObject()) {
      return v8::Function::New(context, noop).ToLocalChecked();
    }

    v8::Local<v8::Object> exportsObject = localExports->ToObject();
    v8::Local<v8::String> key = v8::String::NewFromUtf8(isolate, name);

    if (!exportsObject->Has(key)) {
      return v8::Function::New(context, noop).ToLocalChecked();
    }

    v8::Local<v8::Value> field = exportsObject->Get(key);

    if (!field->IsFunction()) {
      return v8::Function::New(context, noop).ToLocalChecked();
    }

    return v8::Local<v8::Function>::Cast(field);
  }

  void Module::CallExportedFunction(const char * name) {
    v8::Context::Scope context_scope(context);
    v8::Local<v8::Function> func = getExportedFunction(name);

    func->Call(func, 0, NULL);
  }

  void Module::CallExportedFunction(const char * name, unsigned param) {
    v8::Context::Scope context_scope(context);
    v8::Local<v8::Function> func = getExportedFunction(name);
    v8::Handle<v8::Value> args[1];

    args[0] = v8::Number::New(isolate, param);

    func->Call(func, 1, args);
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
}
