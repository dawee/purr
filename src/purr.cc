#include <iostream>

#include <SDL2/SDL.h>
#include <v8/v8.h>
#include <v8/libplatform/libplatform.h>
#include <filesystem/path.h>
#include <filesystem/resolver.h>

#include "game.h"

int main(int argc, char * argv[]) {
  if (argc != 2) {
    std::cerr <<  "usage:" << std::endl << "  purr <filename>" << std::endl;
    return 1;
  }

  filesystem::path filePath(argv[1]);
  std::string mainFilename = filePath.make_absolute().str();

  purr::Game::CreateInstance(mainFilename)->RunLoop();
  purr::Game::DeleteInstance();
  return 0;
}
