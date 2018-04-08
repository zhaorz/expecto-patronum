#include <string>

#include <SFML/Graphics.hpp>

using std::cout;
using std::endl;
using std::string;

namespace Game {

  class Character {
  public:

    Character ( const sf::Vector2f& position, const string& pathToSprite )
      : position(position)
    {
      if ( !texture.loadFromFile(pathToSprite) ) {
        cout << "Error loading Character texture" << endl;
      }

      sprite.setTexture(texture);
      sprite.setPosition(position);
    }

    void update() {
    }

    const sf::Sprite& getSprite() {
      return sprite;
    }


  private:

    sf::Vector2f position;

    sf::Texture texture;
    sf::Sprite  sprite;

  };


};
