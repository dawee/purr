#include "job.h"

namespace purr {
  Job::Job(unsigned type) : type(type) {}
  void Job::Run() {}

  LoadTextureJob::LoadTextureJob(Texture * texture) : Job(JobType::LOAD_TEXTURE), texture(texture) {}

  void LoadTextureJob::Run() {
    texture->Load();
  }

  NoopJob::NoopJob() : Job(JobType::NOOP) {}
  void NoopJob::Run() {}
}
