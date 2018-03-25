#include <iostream>
#include <string>
#include <fstream>
#include <filesystem/path.h>
#include <filesystem/resolver.h>

#include "module.h"
#include "project.h"
#include "util.h"

namespace purr {
  void Module::ExportsGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Value> exports = Project::Instance()->GetModuleFromRoot(info.Holder())->GetExports();
    info.GetReturnValue().Set(exports);
  }

  void Module::ExportsSetter(v8::Local<v8::String > property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info) {
    Module * module = Project::Instance()->GetModuleFromRoot(info.Holder());
    module->exports.Reset(module->isolate, value);
  }

  void Module::ModuleGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Object> module = info.Holder();
    info.GetReturnValue().Set(module);
  }

  Module::Module(v8::Isolate * isolate, std::string filename) : isolate(isolate), filename(filename) {
    v8::Local<v8::ObjectTemplate> moduleTemplate = v8::ObjectTemplate::New(isolate);

    moduleTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "exports"), &ExportsGetter, &ExportsSetter);
    moduleTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "module"), &ModuleGetter);

    context = v8::Context::New(isolate, NULL, moduleTemplate);
    v8::Context::Scope context_scope(context);

    v8::Local<v8::String> filenameUTF8 = v8::String::NewFromUtf8(
      isolate,
      filename.c_str(),
      v8::NewStringType::kNormal
    ).ToLocalChecked();

    v8::Local<v8::Object> localExports = v8::Object::New(isolate);
    exports.Reset(isolate, localExports);

    context->Global()->Set(v8::String::NewFromUtf8(isolate, "__filename__"), filenameUTF8);
    Project::Instance()->FeedContextAPI(context);
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

    v8::Context::Scope context_scope(context);

    v8::Local<v8::String> source = v8::String::NewFromUtf8(
      isolate,
      scriptSource.c_str(),
      v8::NewStringType::kNormal
    ).ToLocalChecked();

    v8::Script::Compile(context, source).ToLocalChecked()->Run(context);
  }

  void Module::CallExportedFunction(const char * name) {
    v8::Context::Scope context_scope(context);
    v8::Local<v8::Value> localExports = v8::Local<v8::Value>::New(isolate, exports);

    if (!localExports->IsObject()) {
      return;
    }

    v8::Local<v8::Object> exportsObject = localExports->ToObject();
    v8::Local<v8::String> key = v8::String::NewFromUtf8(isolate, name);

    if (!exportsObject->Has(key)) {
      return;
    }

    v8::Local<v8::Value> field = exportsObject->Get(key);

    if (!field->IsFunction()) {
      return;
    }

    v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(field);

    func->Call(func, 0, NULL);
  }

  v8::Local<v8::Value> Module::GetExports() {
    return v8::Local<v8::Value>::New(isolate, exports);
  }

  std::string Module::GetFilenameFromRoot(v8::Local<v8::Object> root) {
    v8::Local<v8::Value> filename = root->Get(v8::String::NewFromUtf8(root->GetIsolate(), "__filename__"));

    return ValueToSTDString(filename);
  }

}
