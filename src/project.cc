#include "project.h"

namespace purr {
  Project * Project::instance = NULL;

  Project::Project(v8::Isolate * isolate) : isolate(isolate) {
    api = new API(isolate);
    console = new Console(isolate);
    display = new Display();
  }

  Project::~Project() {
    delete api;
    delete console;
    delete display;
  }

  Project * Project::Instance() {
    return instance;
  }

  Project * Project::CreateInstance() {
    instance = new Project(v8::Isolate::GetCurrent());

    return instance;
  }

  Module * Project::GetModuleFromRoot(v8::Local<v8::Object> root) {
    std::string filename = Module::GetFilenameFromRoot(root);

    return modules[filename];
  }

  Module * Project::SaveModule(std::string filename) {
    if (modules.count(filename) == 0) {
      modules[filename] = new Module(isolate, filename);
      modules[filename]->Run();
    }

    return modules[filename];
  }

  void Project::FeedContextAPI(v8::Local<v8::Context> context) {
    v8::Local<v8::Object> object = context->Global();

    api->FeedObject("purr", object);
    console->FeedObject("console", object);
  }

  void Project::Draw() {
    display->Draw();
  }

  void Project::DeleteInstance() {
    delete instance;
  }

  void Project::HideDisplay() {
    display->Hide();
  }
}
