#ifndef __medialibrary_hpp__
#define __medialibrary_hpp__

#include <SFML/Graphics.hpp>
#include <stdio.h>

class MediaLibrary
{
  private:
    static std::map<std::string, sf::Image> _imageLibrary;
    static std::map<std::string, sf::Font> _fontLibrary;
  public:
    static sf::Image* LoadImage(std::string path);
    static sf::Font* LoadFont(std::string path);
    static void RegisterDefaultFont();
};

#endif
