#pragma once

#include <string>

#include <SFML/Graphics.hpp>

#include "Colors.H"


using std::cout;
using std::endl;
using std::function;
using std::string;

namespace Game {

  class WandDisplay {
  public:

    const int width = 250;
    const int height = 130;
    const int radius = 15;

    WandDisplay ( float cx, float cy )
      : bounds(cx - width / 2., cy - height / 2., width, height)
      , timeout(interval)
    {
      rectangle.setPosition(bounds.left, bounds.top);
      rectangle.setSize(sf::Vector2f(bounds.width, bounds.height));
      rectangle.setFillColor(sf::Color(0, 0, 0, 0));
      rectangle.setOutlineThickness(5);
      rectangle.setOutlineColor(sf::Color(255, 255, 255, 255));

      circle.setRadius(radius);
      circle.setOrigin(radius, radius);
      circle.setPosition(cx, cy);

      warningText.setString("Point your wand\ntowards the screen");
      warningText.setCharacterSize(45);
      warningText.setFillColor(Colors::Orange);
    }

    void setFont ( const sf::Font& font ) {
      warningText.setFont(font);
      auto box = warningText.getGlobalBounds();
      warningText.setPosition(bounds.left + bounds.width / 2. - box.width / 2.,
                              bounds.top - bounds.height);
    }

    void update ( float elapsedTime ) {
      timeout -= elapsedTime;

      if ( timeout < 0. ) {
        shouldWarn = true;
        rectangle.setOutlineColor(Colors::Orange);
        circle.setFillColor(Colors::Orange);
      } else {
        shouldWarn = false;
        rectangle.setOutlineColor(sf::Color(255, 255, 255, 255));
        circle.setFillColor(sf::Color(255, 255, 255, 255));
      }
    }

    void draw ( std::shared_ptr<sf::RenderWindow> window ) {
      window->draw(rectangle);
      window->draw(circle);

      if ( shouldWarn ) {
        window->draw(warningText);
      }
    }

    void updateWandPoint( float x, float y ) {
      circle.setPosition(bounds.left + width * x,
                         bounds.top + height * y);
      timeout = interval;
    }

  private:

    float interval = 1.5;        // Number of seconds before the warning comes up
    float timeout;

    bool shouldWarn = false;

    // Container size
    sf::FloatRect bounds;

    sf::RectangleShape rectangle;
    sf::CircleShape circle;

    sf::Text warningText;

  };

};
