#include <iostream>
#include <thread>

#include <SFML/Graphics.hpp>

#include "cxxopts.hpp"

#include "Game.H"
#include "WandInput.H"


using std::thread;


int main ( int argc, char** argv )
{
  cxxopts::Options options("Expecto Patronum", "Fight Voldemort");
  options.add_options()
    ("h,help", "Show help")
    ("d,debug", "Enable debugging")
    ("s,small", "Use a small window")
    ;

  auto args = options.parse(argc, argv);

  bool debug = false;
  if ( args.count("d") ) {
    debug = true;
    cout << "Debug enabled" << endl;
  }

  bool small = false;
  if ( args.count("s") ) {
    small = true;
    cout << "Using small window" << endl;
  }

  int width = 1920;
  int height = 1080;

  std::shared_ptr<sf::RenderWindow> window;
  if ( debug ) {
    window = std::make_shared<sf::RenderWindow>(sf::VideoMode(width, height),
                                                "Expecto Patronum");
  } else if ( small ) {
    window = std::make_shared<sf::RenderWindow>(sf::VideoMode(800, 600),
                                                "Expecto Patronum");
  } else {
    window = std::make_shared<sf::RenderWindow>(sf::VideoMode::getFullscreenModes()[0],
                                                "Expecto Patronum",
                                                sf::Style::Fullscreen);
  }

  window->setFramerateLimit(60);
  window->setKeyRepeatEnabled(false);

  sf::CircleShape shape(20.f);

  shape.setFillColor(sf::Color::Green);

  Wand::WandInput wandInput;

  thread wandInputThread([&] () { wandInput.run(); });

  Game::GameController game(window);

  sf::Clock clock;

  while ( window->isOpen() ) {
    sf::Event event;

    while ( window->pollEvent(event) ) {

      switch (event.type) {
        // window closed
      case sf::Event::Closed:
        window->close();
        break;

        // key pressed
      // case sf::Event::KeyPressed:
      //   std::cout << "KeyPressed : " << event.key.code << std::endl;
      //   if ( event.key.code == sf::Keyboard::R) {
      //     shape.setFillColor(sf::Color::Red);
      //   }
      //   break;

      case sf::Event::MouseButtonPressed:
        game.onMousePress();
        break;

      case sf::Event::KeyPressed:
        // Debugging
        Wand::Event syntheticEvent;

        switch ( event.key.code ) {

        case sf::Keyboard::W:
          syntheticEvent.type = Wand::Event::Jump;
          break;

        case sf::Keyboard::D:
          syntheticEvent.type = Wand::Event::Attack;
          break;

        case sf::Keyboard::A:
          syntheticEvent.type = Wand::Event::Reflect;
          break;

        case sf::Keyboard::Q:
          window->close();
          exit(0);
          break;

        default:
          break;

        }

        game.onWandInput(syntheticEvent);
        break;

        // we don't process other types of events
      default:
        break;
      }

    }

    Wand::Event wandEvent;

    while ( wandInput.pollEvent(wandEvent) ) {
      game.onWandInput(wandEvent);
    }

    window->clear();

    game.draw();
    game.update(clock.getElapsedTime().asSeconds());
    clock.restart();

    window->display();
  }

  wandInputThread.join();

  return 0;
}
