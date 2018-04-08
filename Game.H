#include <string>

#include <SFML/Graphics.hpp>

#include "Character.H"
#include "WandInput.H"

using std::cout;
using std::endl;
using std::string;

namespace Game {

  const string assetBasePath = "../assets/";

  class GameController {

  public:

    GameController( std::shared_ptr<sf::RenderWindow> window )
      : window(window)
      , harry(sf::Vector2f(0.f, 0.f), assetBasePath + "harry-default.png")
    {

      if ( !backgroundTexture.loadFromFile(assetBasePath + "chamber-of-secrets.png") ) {
        cout << "Error loading background texture" << endl;
      }

      backgroundSprite.setTexture(backgroundTexture);
    }

    void draw() {
      window->draw(backgroundSprite);
      window->draw(harry.getSprite());
    }

    void onMousePress() {
      cout << "GameController : onMousePress" << endl;
    }

    void onWandInput( Wand::Event& event ) {
      cout << "GameController : onWandInput" << endl;
    }

  private:

    std::shared_ptr<sf::RenderWindow> window;

    // Background
    sf::Texture backgroundTexture;
    sf::Sprite  backgroundSprite;

    // Characters
    Character harry;
  };

};
