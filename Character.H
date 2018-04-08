#include <string>

#include <SFML/Graphics.hpp>

using std::cout;
using std::endl;
using std::string;

namespace Game {

  // Pixels per second^2
  const sf::Vector2f gravity = sf::Vector2f(0., 600.f);

  class Character {
  public:

    Character ( const sf::Vector2f& position, const string& pathToSprite )
      : position(position)
      , velocity(0.f, 0.f)
      , maxY(position.y)
      , isJumping(false)
    {
      if ( !texture.loadFromFile(pathToSprite) ) {
        cout << "Error loading Character texture" << endl;
      }

      sprite.setTexture(texture);
      sprite.setPosition(position);
    }

    void update( float elapsedTime ) {
      if ( isJumping ) velocity += elapsedTime * gravity;
      position += elapsedTime * velocity;

      // Snap character back
      if ( position.y > maxY ) {
        position.y = maxY;
        velocity = sf::Vector2f(0.f, 0.f);
        isJumping = false;
      }

      sprite.setPosition(position);
    }

    void jump() {
      if ( isJumping ) return;

      isJumping = true;
      cout << "Character.jump : jumping" << endl;
      velocity = sf::Vector2f(0., -600.f);
    }

    const sf::Sprite& getSprite() {
      return sprite;
    }


  private:

    sf::Vector2f position;

    float maxY;

    // Kinematics
    sf::Vector2f velocity;

    // Jump mechanics
    bool isJumping;

    sf::Texture texture;
    sf::Sprite  sprite;

  };


};
