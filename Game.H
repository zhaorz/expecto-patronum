#include <SFML/Graphics.hpp>

#include "WandInput.H"

using std::cout;
using std::endl;

namespace Game {

  class GameController {

  public:

    GameController( std::shared_ptr<sf::RenderWindow> window )
      : window(window)
    {

      if ( !backgroundTexture.loadFromFile("../assets/chamber-of-secrets.png") ) {
        cout << "Error loading background texture" << endl;
      }

      backgroundSprite.setTexture(backgroundTexture);
    }

    void draw() {
      window->draw(backgroundSprite);
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

  };

};
