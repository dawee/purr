#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

#include "v8/libplatform/libplatform.h"
#include "v8/v8.h"

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("usage:\n\tpurr <filename>\n");
    return 1;
  }

  ifstream scriptFile;

  scriptFile.open(argv[1]);

  if (!scriptFile.is_open()) {
    printf("failed to read '%s'\n", argv[1]);
    return 1;
  }

  string script;

  scriptFile.seekg(0, std::ios::end);
  script.reserve(scriptFile.tellg());
  scriptFile.seekg(0, std::ios::beg);
  script.assign((std::istreambuf_iterator<char>(scriptFile)), std::istreambuf_iterator<char>());
  scriptFile.close();

  v8::Platform* platform = v8::platform::CreateDefaultPlatform();
  v8::V8::InitializePlatform(platform);
  v8::V8::Initialize();
  v8::Isolate::CreateParams create_params;
  create_params.array_buffer_allocator =
  v8::ArrayBuffer::Allocator::NewDefaultAllocator();

  v8::Isolate* isolate = v8::Isolate::New(create_params);
  {
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    v8::Context::Scope context_scope(context);
    v8::Local<v8::String> source =
    v8::String::NewFromUtf8(isolate, script.c_str(), v8::NewStringType::kNormal).ToLocalChecked();
    v8::Local<v8::Script> script =
    v8::Script::Compile(context, source).ToLocalChecked();
    v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
    v8::String::Utf8Value utf8(isolate, result);

    printf("script result: %s\n", *utf8);
  }
  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
  delete create_params.array_buffer_allocator;
  return 0;
}
