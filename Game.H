#pragma once

#include <string>

#include <SFML/Graphics.hpp>

#include "Character.H"
#include "SpellController.H"
#include "Voldemort.H"
#include "WandInput.H"

using std::cout;
using std::endl;
using std::string;

namespace Game {

  const int windowWidth = 1920;
  const int windowHeight = 1080;

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
      , spellController(sf::IntRect(200, 600, 1520, 50),
                        assetBasePath)
      , gameOver(false)
    {
      if ( !backgroundTexture.loadFromFile(assetBasePath + "chamber-of-secrets.png") ) {
        cout << "Error loading background texture" << endl;
      }

      if ( !font.loadFromFile(assetBasePath + "8bit.ttf") ) {
        cout << "Error loading font" << endl;
      }

      harry.setFont(font);
      voldemort.setFont(font);

      spellController.setPlayerHit([&] () { harry.hit(); });
      spellController.setOpponentHit([&] () { voldemort.hit(); });

      harry.setSpellController(spellController);
      voldemort.setSpellController(spellController);

      voldemortController.setAttack([&] () { voldemort.attack(); });
      voldemortController.setJump([&] () { voldemort.jump(); });

      backgroundSprite.setTexture(backgroundTexture);

      gameOverText.setString("Game over");
      gameOverText.setFont(font);
      gameOverText.setCharacterSize(81);
      auto textBounds = gameOverText.getLocalBounds();
      gameOverText.setPosition(sf::Vector2f(windowWidth / 2. - textBounds.width / 2,
                                            windowHeight / 2. - textBounds.height / 2.));
    }

    void draw () {
      window->draw(backgroundSprite);

      harry.draw(window);
      voldemort.draw(window);

      spellController.draw(window);

      if ( gameOver ) {
        window->draw(gameOverText);
      }
    }

    void update ( float elapsedTime ) {
      updateGameOver();

      if ( !gameOver ) {
        harry.update(elapsedTime);
        voldemort.update(elapsedTime);
        voldemortController.update(elapsedTime);
        spellController.update(elapsedTime);
      }
    }

    void updateGameOver () {
      if ( !harry.alive() || !voldemort.alive() ) {
        gameOver = true;
      }
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

    // CPU player
    Voldemort voldemortController;

    SpellController spellController;

    bool gameOver;
    sf::Text gameOverText;

    // Shared assets
    sf::Font font;
  };

};
