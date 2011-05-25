#ifndef __medialibrary_hpp__
#define __medialibrary_hpp__

#include <SFML/Graphics.hpp>
#include <stdio.h>

class MediaLibrary
{
  private:
    static std::map<std::string, sf::Image> _imageLibrary;
  public:
    static sf::Image* LoadImage(std::string path);
};

#endif
