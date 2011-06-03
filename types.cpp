#include "types.hpp"
long fsize(FILE * file)
{
    if(fseek(file, 0, SEEK_END))
        return -1;

    long size = ftell(file);
    if(size < 0)
        return -1;

    if(fseek(file, 0, SEEK_SET))
        return -1;

    return size;
}


void readEntireFile(const char* name, char** str, int* length) {
  printf("reading %s\n", name);
  int* error = 0;
  FILE * file = NULL;
  size_t read = 0;
  *str = NULL;
  if(error) *error = 1;

  do
  {
    file = fopen(name, "rb");
    if(!file) break;

    long size = fsize(file);
    *length = size;
    if(size < 0) break;

    if(error) *error = 0;

    *str = (char*)malloc((size_t)size + 1);
    if(!*str) break;

    read = fread(*str, 1, (size_t)size, file);
    (*str)[read] = 0;
    *str = (char*)realloc(*str, read + 1);

    if(error) *error = (size != (long)read);
  }
  while(0);

  if(file) fclose(file);
  printf("done reading %s\n", name);
}

bool fileExists(const char * filename) {
  if (FILE * file = fopen(filename, "r")) {
    fclose(file);
    return true;
  }
  return false;
}
