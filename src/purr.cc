#include <v8/v8.h>
#include <v8/libplatform/libplatform.h>
#include <filesystem/path.h>
#include <filesystem/resolver.h>

#include "project.h"

int main(int argc, char * argv[]) {
  if (argc != 2) {
     printf("usage:\n\tpurr <filename>\n");
     return 1;
   }

   filesystem::path filePath(argv[1]);

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
     purr::Project::Instance()->Require(filePath.make_absolute().str());
   }

   v8::V8::Dispose();
   v8::V8::ShutdownPlatform();
   delete create_params.array_buffer_allocator;

   return 0;
 }
