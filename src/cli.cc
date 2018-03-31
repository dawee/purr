#include <iostream>
#include <filesystem/path.h>
#include <filesystem/resolver.h>

#include "engine.h"

int main(int argc, char * argv[]) {
  if (argc != 2) {
    std::cerr <<  "usage:" << std::endl << "  purr <filename>" << std::endl;
    return 1;
  }

  filesystem::path filePath(argv[1]);
  purr::Engine engine(filePath.make_absolute().str());

  engine.RunLoop();
  return 0;
}
