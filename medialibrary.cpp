#include "medialibrary.hpp"

std::map <std::string, sf::Image> MediaLibrary::_imageLibrary;


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
