#pragma once

#include <functional>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

using std::cout;
using std::endl;
using std::function;
using std::string;
using std::vector;

namespace Game {

  class Spell {
  public:

    Spell ( const sf::Vector2f& position,
            const sf::Texture& texture )
      : width(texture.getSize().x)
      , left(0.f)
      , top(0.f)
      , height(texture.getSize().y)
      , totalElapsedTime(0.f)
    {
      sprite.setTexture(texture);
      sprite.setTextureRect(sf::IntRect(left, top, w, h));
      sprite.setPosition(position);
      sprite.scale(4.f, 4.f);
    }

    void update ( float elapsedTime ) {
      totalElapsedTime += elapsedTime;

      if ( totalElapsedTime > framePeriod ) {
        totalElapsedTime -= framePeriod;

        left = (left + w) % width;
        sprite.setTextureRect(sf::IntRect(left, top, w, h));
      }
    }

    void draw ( std::shared_ptr<sf::RenderWindow> window ) {
      window->draw(sprite);
    }

  private:

    static constexpr float framePeriod = 1. / 24; // 24 fps
    float totalElapsedTime;

    // Size of a single sprite on the sprite sheet
    static const int w = 64;
    static const int h = 64;

    // Size of the entire sprite sheet
    int width;
    int height;

    int top;
    int left;

    sf::Sprite sprite;

  };

  class SpellController {
  public:

    SpellController ( const sf::IntRect& bbox,
                      const string& assetBasePath )
      : bbox(bbox)
      , playerSpellOrigin(sf::Vector2f(bbox.left, bbox.top))
      , opponentSpellOrigin(sf::Vector2f(bbox.left + bbox.width, bbox.top))
    {
      if ( !attackTexture.loadFromFile(assetBasePath + "attack-spell.png") ) {
        cout << "Error loading attack spell texture" << endl;
      }

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
      Spell spell(playerSpellOrigin, attackTexture);
      playerSpells.push_back(std::move(spell));

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
      for ( auto &spell : playerSpells ) {
        spell.update(elapsedTime);
      }
    }

    void draw( std::shared_ptr<sf::RenderWindow> window ) {
      window->draw(background);

      for ( auto &spell : playerSpells ) {
        spell.draw(window);
      }
    }

  private:

    sf::Vector2f playerSpellOrigin;
    sf::Vector2f opponentSpellOrigin;

    vector<Spell> playerSpells;

    function<void()> playerHitCb;
    function<void()> opponentHitCb;

    sf::IntRect bbox;

    sf::RectangleShape background;

    // Attack
    sf::Texture attackTexture;

  };

};
