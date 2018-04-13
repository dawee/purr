#include "job.h"

namespace purr {
  NoopJob::NoopJob() {}

  JobStatus NoopJob::Run(v8::Persistent<v8::Value>& exception) {
    return JOB_STATUS_OK;
  }

  LoadTextureJob::LoadTextureJob(
    SDLDisplay * display,
    Texture * texture
  ) : display(display), texture(texture) {}

  JobStatus LoadTextureJob::Run(v8::Persistent<v8::Value>& exception) {
    texture->Load(display);
    return JOB_STATUS_OK;
  }

  DestroyTextureJob::DestroyTextureJob(Texture * texture) : texture(texture) {}

  JobStatus DestroyTextureJob::Run(v8::Persistent<v8::Value>& exception) {
    delete texture;
    return JOB_STATUS_OK;
  }

  RaiseExceptionJob::RaiseExceptionJob(v8::Isolate * isolate, v8::Local<v8::Value> exception) {
    this->exception.Reset(isolate, exception);
  }

  JobStatus RaiseExceptionJob::Run(v8::Persistent<v8::Value>& exception) {
    return JOB_STATUS_EXCEPTION;
  }
}
