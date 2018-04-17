#pragma once

#include <string>

#include <SFML/Graphics.hpp>

#include "Character.H"
#include "SpellController.H"
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
      , harry(sf::IntRect(0, 0, 960, 1080),
              assetBasePath,
              assetBasePath + "harry-default.png",
              "Harry")
      , voldemort(sf::IntRect(960, 0, 960, 1080),
                  assetBasePath,
                  assetBasePath + "voldemort-default.png",
                  "Voldemort",
                  true)         // Voldemort is reversed
      , spellController(sf::IntRect(100, 600, 1720, 50))
    {

      if ( !backgroundTexture.loadFromFile(assetBasePath + "chamber-of-secrets.png") ) {
        cout << "Error loading background texture" << endl;
      }

      if ( !font.loadFromFile(assetBasePath + "8bit.ttf") ) {
        cout << "Error loading font" << endl;
      }

      harry.setFont(font);
      voldemort.setFont(font);

      spellController.setPlayerHit(std::bind(&Character::hit, harry));
      spellController.setOpponentHit(std::bind(&Character::hit, voldemort));

      harry.setSpellController(spellController);
      voldemort.setSpellController(spellController);

      backgroundSprite.setTexture(backgroundTexture);
    }

    void draw() {
      window->draw(backgroundSprite);

      harry.draw(window);
      voldemort.draw(window);

      spellController.draw(window);
    }

    void update( float elapsedTime ) {
      harry.update(elapsedTime);
      spellController.update(elapsedTime);
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
        harry.attack();
        break;

      case Wand::Event::Reflect:
        cout << "GameController.onWandInput : Reflect" << endl;
        harry.reflect();
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
    Character voldemort;

    SpellController spellController;

    // Shared assets
    sf::Font font;
  };

};
