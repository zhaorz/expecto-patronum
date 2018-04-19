#pragma once

#include <string>

#include <SFML/Graphics.hpp>

#include "Character.H"
#include "SpellController.H"
#include "Voldemort.H"
#include "WandDisplay.H"
#include "WandInput.H"

using std::cout;
using std::endl;
using std::string;

namespace Game {

  const string assetBasePath = "../assets/";

  class GameController {

  public:

    enum Phase {
      Loading,
      Playing,
      Complete,
    };

    GameController( std::shared_ptr<sf::RenderWindow> window )
      : window(window)
      , width(window->getSize().x)
      , height(window->getSize().y)
      , phase(Loading)
      , loadingTimeout(loadingInterval)
      , ground(height * 0.9)
      , harry(sf::IntRect(0, 0, width / 2, height),
              ground,
              assetBasePath,
              assetBasePath + "hp/",
              "Harry")
      , voldemort(sf::IntRect(width / 2, 0, width / 2, height),
                  ground,
                  assetBasePath,
                  assetBasePath + "vold/",
                  "Voldemort",
                  true)         // Voldemort is reversed
      , spellController(sf::IntRect(0.1 * width, 0.58 * height, 0.8 * width, 50),
                        assetBasePath)
      , wandDisplay(width / 2., height * 0.9)
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

      wandDisplay.setFont(font);

      gameOverText.setString("Game over");
      gameOverText.setFont(font);
      gameOverText.setCharacterSize(81);
      auto textBounds = gameOverText.getLocalBounds();
      gameOverText.setPosition(sf::Vector2f(width / 2. - textBounds.width / 2,
                                            height / 2. - textBounds.height / 2.));

      countdownText.setString(std::to_string((int) std::ceil(loadingTimeout)));
      countdownText.setFont(font);
      countdownText.setCharacterSize(108);
      textBounds = countdownText.getLocalBounds();
      countdownText.setPosition(sf::Vector2f(width / 2. - textBounds.width / 2,
                                             height / 2. - textBounds.height / 2.));

      jumpTutorialText.setString("Wave up to jump");
      jumpTutorialText.setFont(font);
      jumpTutorialText.setCharacterSize(72);
      textBounds = jumpTutorialText.getLocalBounds();
      jumpTutorialText.setPosition(sf::Vector2f(width / 2. - textBounds.width / 2,
                                                height * 0.1));
      jumpTutorialText.setFillColor(sf::Color(72, 255, 157, 255));

      attackTutorialText.setString("Wave right to attack");
      attackTutorialText.setFont(font);
      attackTutorialText.setCharacterSize(72);
      textBounds = attackTutorialText.getLocalBounds();
      attackTutorialText.setPosition(sf::Vector2f(width / 2. - textBounds.width / 2,
                                                  height * 0.8));
      attackTutorialText.setFillColor(sf::Color(255, 102, 63, 255));

      cout << "Game : initialized, dimensions = " << width << "x" << height << endl;
    }

    GameController ( const GameController& other ) = delete;

    void draw () {
      window->draw(backgroundSprite);

      switch ( phase ) {

      case Loading:
        window->draw(jumpTutorialText);
        window->draw(attackTutorialText);
        window->draw(countdownText);
        break;

      case Playing:
        harry.draw(window);
        voldemort.draw(window);
        spellController.draw(window);
        wandDisplay.draw(window);
        break;

      case Complete:
        harry.draw(window);
        voldemort.draw(window);
        spellController.draw(window);
        wandDisplay.draw(window);
        window->draw(gameOverText);
        break;

      }
    }

    void update ( float elapsedTime ) {
      updateGameOver();

      switch ( phase ) {

      case Loading:
        loadingTimeout -= elapsedTime;
        countdownText.setString(std::to_string((int) std::ceil(loadingTimeout)));
        if ( loadingTimeout < 0 ) {
          phase = Playing;
        }
        break;

      case Playing:
        harry.update(elapsedTime);
        voldemort.update(elapsedTime);
        voldemortController.update(elapsedTime);
        spellController.update(elapsedTime);
        wandDisplay.update(elapsedTime);
        break;

      case Complete:
        break;
      }
    }

    void updateGameOver () {
      if ( !harry.alive() || !voldemort.alive() ) {
        phase = Complete;
      }
    }

    void onMousePress() {
      if ( phase == Complete ) {
        phase = Loading;

        harry.reset();
        voldemort.reset();
        spellController.reset();

        loadingTimeout = loadingInterval;
      }
    }

    void onWandInput( Wand::Event& event ) {
      if ( phase != Playing ) return;

      switch ( event.type ) {

      case Wand::Event::WandPoint:
        wandDisplay.updateWandPoint(event.wandPoint.x, event.wandPoint.y);
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

    Phase phase;

    int width;
    int height;
    float ground;               // Baseline of the characters

    const float loadingInterval = 5.;
    float loadingTimeout;

    // Background
    sf::Texture backgroundTexture;
    sf::Sprite  backgroundSprite;

    // Characters
    Character harry;
    Character voldemort;

    // CPU player
    Voldemort voldemortController;

    SpellController spellController;

    WandDisplay wandDisplay;

    sf::Text gameOverText;
    sf::Text countdownText;

    sf::Text jumpTutorialText;
    sf::Text attackTutorialText;

    // Shared assets
    sf::Font font;
  };

};
