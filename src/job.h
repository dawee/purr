#ifndef PURR_JOB_H
#define PURR_JOB_H

#include <v8/v8.h>

#include "display.h"
#include "texture.h"

namespace purr {
  enum JobStatus {
    JOB_STATUS_OK,
    JOB_STATUS_EXCEPTION
  };

  class Job {
    public:
      virtual JobStatus Run(v8::Persistent<v8::Value>&) = 0;
  };

  class NoopJob : public Job {
    public:
      NoopJob();
      JobStatus Run(v8::Persistent<v8::Value>&);
  };

  class LoadTextureJob : public Job {
    private:
      SDLDisplay * display;
      Texture * texture;

    public:
      LoadTextureJob(SDLDisplay * display, Texture *);
      JobStatus Run(v8::Persistent<v8::Value>&);
  };

  class DestroyTextureJob : public Job {
    private:
      Texture * texture;

    public:
      DestroyTextureJob(Texture *);
      JobStatus Run(v8::Persistent<v8::Value>&);
  };

  class RaiseExceptionJob : public Job {
    private:
      v8::Persistent<v8::Value> exception;

    public:
      RaiseExceptionJob(v8::Isolate *, v8::Local<v8::Value>);
      JobStatus Run(v8::Persistent<v8::Value>&);
  };
}

#endif
