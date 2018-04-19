#pragma once

#include <string>

#include <SFML/Graphics.hpp>


using std::cout;
using std::endl;
using std::function;
using std::string;

namespace Game {

  class WandDisplay {
  public:

    const int width = 250;
    const int height = 130;
    const int radius = 10;

    WandDisplay ( float cx, float cy )
      : bounds(cx - width / 2., cy - height / 2., width, height)
    {
      rectangle.setPosition(bounds.left, bounds.top);
      rectangle.setSize(sf::Vector2f(bounds.width, bounds.height));
      rectangle.setFillColor(sf::Color(0, 0, 0, 0));
      rectangle.setOutlineThickness(3);
      rectangle.setOutlineColor(sf::Color(255, 255, 255, 255));

      circle.setRadius(radius);
      circle.setOrigin(radius, radius);
      circle.setPosition(cx, cy);
    }

    void update ( float elapsedTime ) {
    }

    void draw ( std::shared_ptr<sf::RenderWindow> window ) {
      window->draw(rectangle);
      window->draw(circle);
    }

    void updateWandPoint( float x, float y ) {
      circle.setPosition(bounds.left + width * x,
                         bounds.top + height * y);
    }

  private:

    // Container size
    sf::FloatRect bounds;

    sf::RectangleShape rectangle;
    sf::CircleShape circle;

  };

};
