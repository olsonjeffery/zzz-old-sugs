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
