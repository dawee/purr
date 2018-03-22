#include "project.h"

namespace purr {
  Project * Project::instance = NULL;

  Project::Project(v8::Isolate * isolate) : isolate(isolate) {
    moduleTemplate = v8::ObjectTemplate::New(isolate);
    moduleTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "exports"), &Project::ExportsGetter, &Project::ExportsSetter);
    moduleTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "module"), &Project::ModuleGetter);
  }

  Project * Project::Instance() {
    if (instance == NULL) {
      instance = new Project(v8::Isolate::GetCurrent());
    }

    return instance;
  }

  Module * Project::GetModuleFromRoot(v8::Local<v8::Object> root) {
    std::string filename = Module::GetFilenameFromRoot(root);

    return modules[filename];
  }

  Module * Project::Require(std::string filename) {
    if (modules.count(filename) == 0) {
      modules[filename] = new Module(isolate, moduleTemplate, filename);
      modules[filename]->Run();
    }

    return modules[filename];
  }

  void Project::ExportsGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Persistent<v8::Value> * exports = Project::Instance()->GetModuleFromRoot(info.Holder())->GetExports();
    info.GetReturnValue().Set(*exports);
  }

  void Project::ExportsSetter(v8::Local<v8::String > property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info) {
    Project::Instance()->GetModuleFromRoot(info.Holder())->SetExports(value);
  }

  void Project::ModuleGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Object> module = info.Holder();
    info.GetReturnValue().Set(module);
  }

}
