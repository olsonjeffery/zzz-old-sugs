#include "types.hpp"

void readEntireFile(const char* path, char** buffer, int* length) {
  std::ifstream is;
  is.open (path, std::ios::binary );

  // get length of file:
  is.seekg (0, std::ios::end);
  (*length) = is.tellg();
  is.seekg (0, std::ios::beg);

  // allocate memory:
  (*buffer) = new char [*length];

  // read data as a block:
  is.read (*buffer,*length);
  is.close();
}

bool fileExists(const char * filename) {
  if (FILE * file = fopen(filename, "r")) {
    fclose(file);
    return true;
  }
  return false;
}
