#include "job.h"

namespace purr {
  Job::Job(unsigned type) : type(type) {}
  void Job::Run() {}

  NoopJob::NoopJob() : Job(JobType::NOOP) {}
  void NoopJob::Run() {}

  LoadTextureJob::LoadTextureJob(Texture * texture) : Job(JobType::LOAD_TEXTURE), texture(texture) {}

  void LoadTextureJob::Run() {
    texture->Load();
  }

  DestroyTextureJob::DestroyTextureJob(Texture * texture) : Job(JobType::DESTROY_TEXTURE), texture(texture) {}

  void DestroyTextureJob::Run() {
    delete texture;
  }

}
