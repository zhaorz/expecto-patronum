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

  const string assetBasePath = "../assets/";

  class GameController {

  public:

    GameController( std::shared_ptr<sf::RenderWindow> window )
      : window(window)
      , width(window->getSize().x)
      , height(window->getSize().y)
      , ground(height * 0.8)
      , harry(sf::IntRect(0, 0, width / 2, height),
              ground,
              assetBasePath,
              assetBasePath + "hp/HP_Idle_Still.png",
              "Harry")
      , voldemort(sf::IntRect(width / 2, 0, width / 2, height),
                  ground,
                  assetBasePath,
                  assetBasePath + "vold/VOLD_Idle_Still.png",
                  "Voldemort",
                  true)         // Voldemort is reversed
      , spellController(sf::IntRect(0.1 * width, 0.64 * height, 0.8 * width, 50),
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
      spellController.setPlayerIntersect([&] ( const sf::FloatRect& box )
                                         { return harry.intersect(box); });
      spellController.setOpponentIntersect([&] ( const sf::FloatRect& box )
                                           { return voldemort.intersect(box); });

      harry.setSpellController(spellController);
      voldemort.setSpellController(spellController);

      voldemortController.setAttack([&] () { voldemort.attack(); });
      voldemortController.setJump([&] () { voldemort.jump(); });

      auto backgroundSize = backgroundTexture.getSize();
      backgroundSprite.setTexture(backgroundTexture);
      float scaleX = (float) width / backgroundSize.x;
      float scaleY = (float) height / backgroundSize.y;
      float scale = std::max(scaleX, scaleY);
      backgroundSprite.scale(scale, scale);

      gameOverText.setString("Game over");
      gameOverText.setFont(font);
      gameOverText.setCharacterSize(81);
      auto textBounds = gameOverText.getLocalBounds();
      gameOverText.setPosition(sf::Vector2f(width / 2. - textBounds.width / 2,
                                            height / 2. - textBounds.height / 2.));

      cout << "Game : initialized, dimensions = " << width << "x" << height << endl;
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

    int width;
    int height;
    float ground;               // Baseline of the characters

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
