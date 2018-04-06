#include <iostream>
#include <filesystem/path.h>
#include <filesystem/resolver.h>

#include "engine.h"

int main(int argc, char * argv[]) {
  if (argc != 2) {
    std::cerr << "usage:" << std::endl << "  purr <module/package path>" << std::endl;
    return 1;
  }

  std::string entryName(argv[1]);
  purr::Engine engine(entryName, filesystem::path::getcwd().str());

  return engine.RunLoop();
}
