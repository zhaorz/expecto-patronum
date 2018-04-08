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
      , harry(sf::Vector2f(100.f, 500.f), assetBasePath + "harry-default.png")
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

    void update( float elapsedTime ) {
      harry.update(elapsedTime);
    }

    void onMousePress() {
      cout << "GameController : onMousePress" << endl;
    }

    void onWandInput( Wand::Event& event ) {
      switch ( event.type ) {

      case Wand::Event::WandPoint:
        // cout << "GameController.onWandInput : WandPoint" << endl;
        break;

      case Wand::Event::Jump:
        cout << "GameController.onWandInput : Jump" << endl;
        harry.jump();
        break;

      case Wand::Event::Attack:
        cout << "GameController.onWandInput : Attack" << endl;
        break;

      case Wand::Event::Reflect:
        cout << "GameController.onWandInput : Reflect" << endl;
        break;

      case Wand::Event::OutOfScreen:
        cout << "GameController.onWandInput : OutOfScreen" << endl;
        break;

      default:
        cout << "GameController.onWandInput : Unknown event" << endl;
        break;

      }
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
