#pragma once

#include <functional>
#include <string>

#include <SFML/Graphics.hpp>

using std::cout;
using std::endl;
using std::function;
using std::string;

namespace Game {

  class SpellController {
  public:

    SpellController ( const sf::IntRect& bbox )
      : bbox(bbox)
    {
      background.setPosition(sf::Vector2f(bbox.left, bbox.top));
      background.setSize(sf::Vector2f(bbox.width, bbox.height));
      background.setFillColor(sf::Color(126, 247, 210, 128));
    }

    void setPlayerHit ( function<void()> cb ) {
      playerHitCb = cb;
    }

    void setOpponentHit ( function<void()> cb ) {
      opponentHitCb = cb;
    }

    void castPlayerAttack () {
      cout << "SpellController.castPlayerAttack" << endl;
    }

    void castPlayerReflect () {
      cout << "SpellController.castPlayerReflect" << endl;
    }

    void castOpponentAttack () {
      cout << "SpellController.castOpponentAttack" << endl;
    }

    void castOpponentReflect () {
      cout << "SpellController.castOpponentReflect" << endl;
    }

    void update( float elapsedTime ) {
    }

    void draw( std::shared_ptr<sf::RenderWindow> window ) {
      window->draw(background);
    }

  private:

    function<void()> playerHitCb;
    function<void()> opponentHitCb;

    sf::IntRect bbox;

    sf::RectangleShape background;

  };

};
