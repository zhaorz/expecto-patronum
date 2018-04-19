#pragma once

#include <functional>
#include <string>

#include <SFML/Graphics.hpp>

#include "SpellController.H"

using std::cout;
using std::endl;
using std::function;
using std::string;

namespace Game {

  // Pixels per second^2
  const sf::Vector2f gravity = sf::Vector2f(0., 5000.f);
  const sf::Vector2f jumpSpeed = sf::Vector2f(0., -2000.f);

  const float margin = 50.f;

  template<typename T>
  sf::Vector2f midpoint ( const sf::Rect<T>& box ) {
    return sf::Vector2f(box.left + box.width / 2, box.top + box.height / 2);
  }

  class Character {
  public:

    enum State {
      Idle,
      Jump,
      Attack,
      Hit,
      Dead,
    };

    Character ( const sf::IntRect& bbox,
                float ground,
                const string& assetBasePath,
                const string& spriteBasePath,
                const string& name,
                bool isOpponent = false )
      : bbox(bbox)
      , bLeft(bbox.left)
      , bTop(bbox.top)
      , bWidth(bbox.width)
      , bHeight(bbox.height)
      , name(name)
      , nLives(3)
      , velocity(0.f, 0.f)
      , ground(ground)
      , isJumping(false)
      , isOpponent(isOpponent)
    {
      if ( !idleTexture.loadFromFile(spriteBasePath + "idle.png") ||
           !jumpTexture.loadFromFile(spriteBasePath + "jump.png") ||
           !attackTexture.loadFromFile(spriteBasePath + "attack.png") ||
           !hitTexture.loadFromFile(spriteBasePath + "hit.png") ||
           !deadTexture.loadFromFile(spriteBasePath + "dead.png") ) {
        cout << "Error loading Character textures" << endl;
      }

      if ( !heartTexture.loadFromFile(assetBasePath + "heart.png") ) {
        cout << "Error loading Heart texture" << endl;
      }

      // Sprite initialization ----------------------------------------------------------------------

      state = Idle;
      sprite.setTexture(idleTexture);
      sprite.scale(0.8, 0.8);
      auto rect = sprite.getTextureRect();
      auto bounds = sprite.getGlobalBounds();
      cWidth = bounds.width;
      cHeight = bounds.height;

      if ( isOpponent ) {
        position = sf::Vector2f(bLeft + bWidth - bounds.width - 100.f, ground - bounds.height);
      } else {
        position = sf::Vector2f(bLeft + 100.f, ground - bounds.height);
      }

      sprite.setPosition(position);

      heartTexture.setRepeated(true); heartSprite.setTexture(heartTexture);

      // Game elements ------------------------------------------------------------------------------

      nameText.setString(name);
      nameText.setCharacterSize(63);
      nameText.setPosition(sf::Vector2f(bLeft + margin, bTop + margin));

      sf::Vector2u heartSize = heartTexture.getSize();
      heartSprite.setTextureRect(sf::IntRect(0, 0, nLives * heartSize.x, heartSize.y));
      heartSprite.setPosition(sf::Vector2f(bLeft + margin,
                                           bTop + nameText.getLocalBounds().height + 3 * margin));

    }

    void setFont ( const sf::Font& font ) {
      nameText.setFont(font);
    }

    void setSpellController( SpellController& controller ) {
      if ( isOpponent ) {
        castAttack = [&] () { controller.castOpponentAttack(); };
        castReflect = [&] () { controller.castOpponentReflect(); };
      } else {
        castAttack = [&] () { controller.castPlayerAttack(); };
        castReflect = [&] () { controller.castPlayerReflect(); };
      }
    }

    bool alive () {
      return nLives > 0;
    }

    void update ( float elapsedTime ) {
      switch ( state ) {

      case Jump:
        velocity += elapsedTime * gravity;
        position += elapsedTime * velocity;

        // Snap character back
        if ( position.y + cHeight > ground ) {
          position.y = ground - cHeight;
          velocity = sf::Vector2f(0.f, 0.f);
          state = Idle;
          sprite.setTexture(idleTexture);
        }

        sprite.setPosition(position);
        break;

      case Attack:
      case Hit:
        timeout -= elapsedTime;
        if ( timeout < 0 ) {
          state = Idle;
          sprite.setTexture(idleTexture);
        }
        break;

      case Dead:
      case Idle:
        break;
      }
    }

    void jump () {
      if ( state != Idle ) return;

      state = Jump;
      sprite.setTexture(jumpTexture);
      velocity = jumpSpeed;
    }

    bool intersect( const sf::FloatRect& box ) {
      return sprite.getGlobalBounds().contains(midpoint(box));
    }

    void attack () {
      if ( state != Idle ) return;

      state = Attack;
      timeout = attackInterval;
      sprite.setTexture(attackTexture);
      castAttack();
    }

    void reflect () {
      castReflect();
    }

    void draw ( std::shared_ptr<sf::RenderWindow> window ) {
      window->draw(sprite);
      window->draw(nameText);
      window->draw(heartSprite);
    }

    void hit () {
      nLives--;
      sf::Vector2u heartSize = heartTexture.getSize();
      heartSprite.setTextureRect(sf::IntRect(0, 0, nLives * heartSize.x, heartSize.y));

      // Finish the fall if we're jumping
      if ( state != Jump )
        state = Hit;

      timeout = hitInterval;
      sprite.setTexture(hitTexture);

      if ( nLives == 0 ) {
        state = Dead;
        sprite.setTexture(deadTexture);
      }
    }

  private:

    // Container size
    sf::IntRect bbox;
    int bLeft;
    int bTop;
    int bWidth;
    int bHeight;

    // Title
    const string name;
    sf::Text nameText;

    // Health
    int nLives;
    sf::Texture heartTexture;
    sf::Sprite heartSprite;

    // Character size
    int cWidth;
    int cHeight;

    sf::Vector2f position;
    bool isOpponent;

    // Kinematics
    sf::Vector2f velocity;
    float ground;

    // Animation mechanics
    bool isJumping;
    State state;
    float timeout;

    // Animation duration, in seconds
    float attackInterval = 0.4;
    float hitInterval = 0.6;

    // Spell mechanics
    function<void()> castAttack;
    function<void()> castReflect;

    sf::Texture idleTexture;
    sf::Texture jumpTexture;
    sf::Texture attackTexture;
    sf::Texture hitTexture;
    sf::Texture deadTexture;

    sf::Sprite  sprite;


  };


};
