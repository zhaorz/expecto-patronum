#pragma once

#include <cmath>
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

    bool hidden;

    Spell ( const sf::Vector2f& position,
            const sf::Texture& texture,
            const int direction = 1 )
      : width(texture.getSize().x)
      , left(0.f)
      , top(0.f)
      , height(texture.getSize().y)
      , direction(direction)
      , totalElapsedTime(0.f)
      , hidden(false)
    {
      sprite.setTexture(texture);
      sprite.setTextureRect(sf::IntRect(left, top, w, h));
      sprite.setPosition(position);
      sprite.scale(4.f, 4.f);
    }

    const sf::Vector2f& getPosition() {
      return sprite.getPosition();
    }

    sf::FloatRect getGlobalBounds() {
      return sprite.getGlobalBounds();
    }

    void hide () {
      hidden = true;
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
      if ( !hidden ) {
        window->draw(sprite);
      }
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

  const sf::Vector2f Spell::speed = sf::Vector2f(2000.f, 0.f);

  class Explosion {
  public:

    bool done;

    Explosion ( const sf::Vector2f& position,
                const sf::Texture& texture )
      : width(texture.getSize().x)
      , left(0.f)
      , top(0.f)
      , height(texture.getSize().y)
      , totalElapsedTime(0.f)
      , done(false)
    {
      sprite.setTexture(texture);
      sprite.setTextureRect(sf::IntRect(left, top, w, h));
      sprite.setPosition(position);
    }

    const sf::Vector2f& getPosition() {
      return sprite.getPosition();
    }

    sf::FloatRect getGlobalBounds() {
      return sprite.getGlobalBounds();
    }

    void update ( float elapsedTime ) {
      totalElapsedTime += elapsedTime;

      // Update the sprite frame
      if ( totalElapsedTime > framePeriod ) {
        totalElapsedTime -= framePeriod;

        left = (left + w) % width;

        if ( left == 0 ) {
          top = (top + h) % height;

          // Wraparound to top again
          if ( top == 0 ) {
            done = true;
          }
        }

        sprite.setTextureRect(sf::IntRect(left, top, w, h));
      }
    }

    void draw ( std::shared_ptr<sf::RenderWindow> window ) {
      if ( !done ) {
        window->draw(sprite);
      }
    }

  private:

    static constexpr float framePeriod = 1. / 24; // 24 fps
    float totalElapsedTime;

    // Size of a single sprite on the sprite sheet
    static const int w = 256;
    static const int h = 256;

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
      , opponentSpellOrigin(sf::Vector2f(bbox.left + bbox.width - spellWidth, bbox.top))
    {
      if ( !playerAttackTexture.loadFromFile(assetBasePath + "attack-spell.png") ) {
        cout << "Error loading attack spell texture" << endl;
      }

      if ( !opponentAttackTexture.loadFromFile(assetBasePath + "attack-spell-green.png") ) {
        cout << "Error loading attack spell texture" << endl;
      }

      if ( !explosionTexture.loadFromFile(assetBasePath + "explosion71.png") ) {
        cout << "Error loading explosion texture" << endl;
      }
    }

    void setPlayerHit ( function<void()> cb ) {
      playerHitCb = cb;
    }

    void setOpponentHit ( function<void()> cb ) {
      opponentHitCb = cb;
    }

    void setPlayerIntersect ( function<bool(const sf::FloatRect&)> cb ) {
      playerIntersect = cb;
    }

    void setOpponentIntersect ( function<bool(const sf::FloatRect&)> cb ) {
      opponentIntersect = cb;
    }

    void castPlayerAttack () {
      Spell spell(playerSpellOrigin, playerAttackTexture);
      playerSpells.push_back(std::move(spell));
    }

    void castOpponentAttack () {
      Spell spell(opponentSpellOrigin, opponentAttackTexture, -1);
      opponentSpells.push_back(std::move(spell));
    }

    void castPlayerReflect () {
      cout << "SpellController.castPlayerReflect" << endl;
    }

    void castOpponentReflect () {
      cout << "SpellController.castOpponentReflect" << endl;
    }

    void update( float elapsedTime ) {
      auto it = playerSpells.begin();
      while (it != playerSpells.end()) {
        it->update(elapsedTime);

        if ( opponentIntersect(it->getGlobalBounds()) ) {
          opponentHitCb();
          it = playerSpells.erase(it);
        } else if ( it->hidden ) {
          it = playerSpells.erase(it);
        } else {
          ++it;
        }
      }

      it = opponentSpells.begin();
      while (it != opponentSpells.end()) {
        it->update(elapsedTime);

        if ( playerIntersect(it->getGlobalBounds()) ) {
          playerHitCb();
          it = opponentSpells.erase(it);
        } else if ( it->hidden ) {
          it = opponentSpells.erase(it);
        } else {
          ++it;
        }
      }

      auto eit = explosions.begin();
      while (eit != explosions.end()) {
        eit->update(elapsedTime);

        if ( eit->done ) {
          eit = explosions.erase(eit);
        } else {
          ++eit;
        }
      }

      // Check for intersecting spells
      for ( auto &playerSpell : playerSpells ) {
        if ( playerSpell.hidden ) continue;
        float px = playerSpell.getPosition().x;
        for ( auto &opponentSpell : opponentSpells ) {
          if ( opponentSpell.hidden ) continue;
          float ox = opponentSpell.getPosition().x;

          if ( (px < ox && ox < px + spellWidth)
               || (ox < px && px < ox + spellWidth) ) {
            float center = std::min(px, ox) + std::fabs(px - ox) + spellWidth / 2.f;

            Explosion explosion(sf::Vector2f(center, playerSpellOrigin.y), explosionTexture);
            explosions.push_back(std::move(explosion));

            playerSpell.hide();
            opponentSpell.hide();
            break;
          }
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

      for ( auto &explosion : explosions ) {
        explosion.draw(window);
      }
    }

  private:

    sf::Vector2f playerSpellOrigin;
    sf::Vector2f opponentSpellOrigin;

    vector<Spell> playerSpells;
    vector<Spell> opponentSpells;
    vector<Explosion> explosions;

    function<void()> playerHitCb;
    function<void()> opponentHitCb;

    function<bool(const sf::FloatRect&)> playerIntersect;
    function<bool(const sf::FloatRect&)> opponentIntersect;

    sf::IntRect bbox;

    static constexpr float spellWidth = 200;

    // Assets
    sf::Texture playerAttackTexture;
    sf::Texture opponentAttackTexture;
    sf::Texture explosionTexture;

  };

};
