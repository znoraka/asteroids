#include <iostream>
#include <functional>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <ncurses.h>
#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>

sf::RenderWindow *window;

#include "neuron.h"
#include "network.h"
#include "game.h"

int main(int argc, char **argv) {
  std::srand(std::time(0));
  window = new sf::RenderWindow(sf::VideoMode(800, 600), "My window");
  
  std::vector<int> dimensions{32, 20, 10, 2};

  WINDOW *w = initscr();
  cbreak();
  nodelay(w, TRUE);

  int speed = 30;

  game->createNetworks(dimensions);
  // char ch;
  for(;;) {
    // ch = getch();
    // if(ch == 65) speed -= 5;
    // if(ch == 66) speed += 5;
    sf::Event event;
    while (window->pollEvent(event)) {
      if (event.type == sf::Event::Closed)
	window->close();
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
      speed--;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
      speed++;
    }

    speed = std::max(0, speed);

    if(speed > 0)
      window->clear(sf::Color::Black);

    game->update(speed);

    if(speed > 0)
      window->display();

    mvprintw(3, 0, "speed = %d", speed);
    refresh();
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    clear();
  }
  endwin();
}
