#include "graphics.hpp"

graphicsEnv initGraphics() {
  sf::RenderWindow* win = new sf::RenderWindow(sf::VideoMode(640,480, 32), "reFormer");

  graphicsEnv gfxEnv = { win };
  return gfxEnv;
}

void teardownGraphics(sf::RenderWindow* window) {
  delete window;
}

static std::map <char*, sf::Image> _imageLibrary;
sf::Image* loadImage(char* path) {
  sf::Image* img = 0;
  if (_imageLibrary.find(path) == _imageLibrary.end()) {
    sf::Image newImg;
    printf("image not in library cache...\n");
    if(!newImg.LoadFromFile(path)) {
      return 0;
    }
    _imageLibrary[path] = newImg;
  }
  else {
    printf("image in library cache...\n");
  }

  img = &(*(_imageLibrary.find(path))).second;

  return img;
}
