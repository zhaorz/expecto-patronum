#include <string>

#include <SFML/Graphics.hpp>

using std::cout;
using std::endl;
using std::string;

namespace Game {

  // Pixels per second^2
  const sf::Vector2f gravity = sf::Vector2f(0., 2000.f);

  class Character {
  public:

    Character ( const sf::IntRect& bbox,
                const string& pathToSprite,
                bool reversed = false )
      : bbox(bbox)
      , bLeft(bbox.left)
      , bTop(bbox.top)
      , bWidth(bbox.width)
      , bHeight(bbox.height)
      , velocity(0.f, 0.f)
      , ground(900.f)
      , isJumping(false)
      , reversed(reversed)
    {
      if ( !texture.loadFromFile(pathToSprite) ) {
        cout << "Error loading Character texture" << endl;
      }

      sprite.setTexture(texture);
      sf::IntRect rect = sprite.getTextureRect();
      cWidth = rect.width;
      cHeight = rect.height;

      if ( reversed ) {
        sprite.setTextureRect(sf::IntRect(rect.width, 0, -rect.width, rect.height));
        position = sf::Vector2f(bLeft + bWidth - rect.width - 100.f, ground - rect.height);
      } else {
        position = sf::Vector2f(bLeft + 100.f, ground - rect.height);
      }

      sprite.setPosition(position);
    }

    void update( float elapsedTime ) {
      if ( isJumping ) velocity += elapsedTime * gravity;
      position += elapsedTime * velocity;

      // Snap character back
      if ( position.y + cHeight > ground ) {
        position.y = ground - cHeight;
        velocity = sf::Vector2f(0.f, 0.f);
        isJumping = false;
      }

      sprite.setPosition(position);
    }

    void jump() {
      if ( isJumping ) return;

      isJumping = true;
      cout << "Character.jump : jumping" << endl;
      velocity = sf::Vector2f(0., -1000.f);
    }

    const sf::Sprite& getSprite() {
      return sprite;
    }


  private:

    // Container size
    sf::IntRect bbox;
    int bLeft;
    int bTop;
    int bWidth;
    int bHeight;

    // Character size
    int cWidth;
    int cHeight;

    sf::Vector2f position;
    bool reversed;

    // Kinematics
    sf::Vector2f velocity;

    // Jump mechanics
    bool isJumping;
    float ground;

    sf::Texture texture;
    sf::Sprite  sprite;

  };


};
