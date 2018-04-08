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
    std::make_shared<sf::RenderWindow>(sf::VideoMode(width, height), "SFML works!");

  sf::CircleShape shape(20.f);

  shape.setFillColor(sf::Color::Green);

  Wand::WandInput wandInput;

  thread wandInputThread([&] () { wandInput.run(); });

  Game::GameController game(window);


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

        // we don't process other types of events
      default:
        break;
      }

    }

    Wand::Event wandEvent;

    while ( wandInput.pollEvent(wandEvent) ) {

      switch (wandEvent.type) {
      case Wand::Event::WandPoint:

        shape.setPosition(wandEvent.wandPoint.x * width, wandEvent.wandPoint.y * height);

        game.onWandInput(wandEvent);

        break;

      default:
        break;
      }
    }

    window->clear();
    game.draw();
    window->display();
  }

  wandInputThread.join();

  return 0;
}
