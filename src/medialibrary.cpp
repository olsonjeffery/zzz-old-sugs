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

#include "medialibrary.hpp"

std::map <std::string, sf::Image> MediaLibrary::_imageLibrary;
std::map <std::string, sf::Font> MediaLibrary::_fontLibrary;

sf::Image* MediaLibrary::LoadImage(std::string path) {
  sf::Image* img = 0;
  if (MediaLibrary::_imageLibrary.find(path) == MediaLibrary::_imageLibrary.end()) {
    sf::Image newImg;
    printf("image not in library cache...\n");
    if(!newImg.LoadFromFile(path)) {
      return 0;
    }
    MediaLibrary::_imageLibrary[path] = newImg;
  }
  else {
    printf("image in library cache...\n");
  }

  img = &(*(MediaLibrary::_imageLibrary.find(path))).second;

  return img;
}
sf::Font* MediaLibrary::LoadFont(std::string path) {
  sf::Font* font = 0;
  if (MediaLibrary::_fontLibrary.find(path) == MediaLibrary::_fontLibrary.end()) {
    sf::Font newFont;
    printf("font not in library cache...\n");
    if(!newFont.LoadFromFile(path)) {
      return 0;
    }
    MediaLibrary::_fontLibrary[path] = newFont;
  }
  else {
    printf("font in library cache...\n");
  }

  font = &(*(MediaLibrary::_fontLibrary.find(path))).second;

  return font;
}

void MediaLibrary::RegisterDefaultFont() {
    sf::Font defFont = sf::Font::GetDefaultFont();
    std::string defaultStr("default");
    _fontLibrary[defaultStr] = defFont;
}
