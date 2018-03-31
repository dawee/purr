#include <iostream>
#include <filesystem/path.h>
#include <filesystem/resolver.h>

#include "game.h"

int main(int argc, char * argv[]) {
  if (argc != 2) {
    std::cerr <<  "usage:" << std::endl << "  purr <filename>" << std::endl;
    return 1;
  }

  filesystem::path filePath(argv[1]);
  purr::Game game(filePath.make_absolute().str());

  game.RunLoop();
  return 0;
}
