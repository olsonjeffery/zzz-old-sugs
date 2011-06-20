/* Copyright 2011 Jeffery Olson <olson.jeffery@gmail.com>. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list of
 *     conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice, this list
 *     of conditions and the following disclaimer in the documentation and/or other materials
 *     provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY JEFFERY OLSON <OLSON.JEFFERY@GMAIL.COM> ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * JEFFERY OLSON <OLSON.JEFFERY@GMAIL.COM> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of Jeffery Olson <olson.jeffery@gmail.com>.
 *
 */

#include "common.hpp"

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
