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
            const sf::Texture& texture,
            const int direction = 1 )
      : width(texture.getSize().x)
      , left(0.f)
      , top(0.f)
      , height(texture.getSize().y)
      , direction(direction)
      , totalElapsedTime(0.f)
    {
      sprite.setTexture(texture);
      sprite.setTextureRect(sf::IntRect(left, top, w, h));
      sprite.setPosition(position);
      sprite.scale(4.f, 4.f);
    }

    const sf::Vector2f& getPosition() {
      return sprite.getPosition();
    }

    void update ( float elapsedTime ) {

      sprite.move(direction * elapsedTime * speed);

      totalElapsedTime += elapsedTime;

      // Update the sprite frame
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

    // Either +1 or -1
    int direction;

    sf::Sprite sprite;

    static const sf::Vector2f speed;  // Pixels per second

  };

  const sf::Vector2f Spell::speed = sf::Vector2f(1000.f, 0.f);

  class SpellController {
  public:

    SpellController ( const sf::IntRect& bbox,
                      const string& assetBasePath )
      : bbox(bbox)
      , playerSpellOrigin(sf::Vector2f(bbox.left, bbox.top))
      , opponentSpellOrigin(sf::Vector2f(bbox.left + bbox.width - 100, bbox.top))
    {
      if ( !playerAttackTexture.loadFromFile(assetBasePath + "attack-spell.png") ) {
        cout << "Error loading attack spell texture" << endl;
      }

      if ( !opponentAttackTexture.loadFromFile(assetBasePath + "attack-spell-green.png") ) {
        cout << "Error loading attack spell texture" << endl;
      }
    }

    void setPlayerHit ( function<void()> cb ) {
      playerHitCb = cb;
    }

    void setOpponentHit ( function<void()> cb ) {
      opponentHitCb = cb;
    }

    void castPlayerAttack () {
      cout << "SpellController.castPlayerAttack" << endl;
      Spell spell(playerSpellOrigin, playerAttackTexture);
      playerSpells.push_back(std::move(spell));
    }

    void castPlayerReflect () {
      cout << "SpellController.castPlayerReflect" << endl;
    }

    void castOpponentAttack () {
      cout << "SpellController.castOpponentAttack" << endl;
      Spell spell(opponentSpellOrigin, opponentAttackTexture, -1);
      opponentSpells.push_back(std::move(spell));
    }

    void castOpponentReflect () {
      cout << "SpellController.castOpponentReflect" << endl;
    }

    void update( float elapsedTime ) {
      auto it = playerSpells.begin();
      while (it != playerSpells.end()) {
        it->update(elapsedTime);

        if ( it->getPosition().x > bbox.left + bbox.width ) {
          opponentHitCb();
          it = playerSpells.erase(it);
        } else {
          ++it;
        }
      }

      it = opponentSpells.begin();
      while (it != opponentSpells.end()) {
        it->update(elapsedTime);

        if ( it->getPosition().x < bbox.left ) {
          playerHitCb();
          it = opponentSpells.erase(it);
        } else {
          ++it;
        }
      }
    }

    void draw( std::shared_ptr<sf::RenderWindow> window ) {
      for ( auto &spell : playerSpells ) {
        spell.draw(window);
      }

      for ( auto &spell : opponentSpells ) {
        spell.draw(window);
      }
    }

  private:

    sf::Vector2f playerSpellOrigin;
    sf::Vector2f opponentSpellOrigin;

    vector<Spell> playerSpells;
    vector<Spell> opponentSpells;

    function<void()> playerHitCb;
    function<void()> opponentHitCb;

    sf::IntRect bbox;

    // Attack
    sf::Texture playerAttackTexture;
    sf::Texture opponentAttackTexture;

  };

};
