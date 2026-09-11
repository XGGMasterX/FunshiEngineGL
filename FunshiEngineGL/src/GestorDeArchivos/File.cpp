#include "File.h"

#include <utility>

File::File(std::string pathName) : pathName(std::move(pathName)) {}
std::string File::getPathRoot() { return pathRoot; }
std::string File::getPathName() { return pathName; }
void File::setPathRoot(std::string root) { pathRoot = std::move(root); }
