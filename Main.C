#include <iostream>
#include <thread>

#include <SFML/Graphics.hpp>

#include "WandInput.H"


using std::thread;


int main()
{
  sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");

  sf::CircleShape shape(100.f);

  shape.setFillColor(sf::Color::Green);

  Wand::WandInput wandInput;

  thread wandInputThread([&] () { wandInput.run(); });


  while ( window.isOpen() ) {
    sf::Event event;

    while ( window.pollEvent(event) ) {

      switch (event.type) {
        // window closed
      case sf::Event::Closed:
        window.close();
        break;

        // key pressed
      case sf::Event::KeyPressed:
        std::cout << "KeyPressed : " << event.key.code << std::endl;
        if ( event.key.code == sf::Keyboard::R) {
          shape.setFillColor(sf::Color::Red);
        }
        break;

        // we don't process other types of events
      default:
        break;
      }

    }

    Wand::Event wandEvent;

    while ( wandInput.pollEvent(wandEvent) ) {

      switch (wandEvent) {
      case Wand::Jump:
        shape.setFillColor(sf::Color::Red);
        break;

      default:
        break;
      }
    }

    window.clear();
    window.draw(shape);
    window.display();
  }

  wandInputThread.join();

  return 0;
}
