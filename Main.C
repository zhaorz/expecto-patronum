#include <iostream>
#include <thread>

#include <SFML/Graphics.hpp>

#include "Game.H"
#include "WandInput.H"


using std::thread;


int main()
{
  int width = 1920;
  int height = 1080;

  std::shared_ptr<sf::RenderWindow> window =
    std::make_shared<sf::RenderWindow>(sf::VideoMode(width, height),
                                       "Expecto Patronum",
                                       sf::Style::Fullscreen);
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
