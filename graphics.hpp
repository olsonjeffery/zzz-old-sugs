#include <SFML/Graphics.hpp>

typedef struct {
  sf::RenderWindow* window;
} graphicsEnv;

graphicsEnv initGraphics();
