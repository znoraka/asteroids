#include <iostream>
#include <functional>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <ncurses.h>
#include <chrono>
#include <thread>

// #define GRAPHICS

#ifdef GRAPHICS
#include <SFML/Graphics.hpp>
sf::RenderWindow *window;
#endif

#define ELITISM 0.4f
bool breedWithBest = false;

#include "neuron.h"
#include "network.h"
#include "game.h"


int main(int argc, char **argv) {
#ifdef GRAPHICS
  window = new sf::RenderWindow(sf::VideoMode(800, 600), "My window");
#endif
  std::vector<int> dimensions;

  for (int i = 2; i < argc; i++) {
    dimensions.push_back(atoi(argv[i]));
  }

  std::string weights = argv[1];

  if(dimensions.size() == 0) dimensions = {16, 10, 2};
  
  WINDOW *w = initscr();
  cbreak();
  nodelay(w, TRUE);

#ifdef GRAPHICS  
  int speed = 30;
#else
  int speed = 0;
#endif

  game->createNetworks(dimensions);

  if(weights.size() > 0)
    game->loadNetwork("weights.txt");
  
  char ch;
  for(;;) {
    ch = getch();
    if(ch == ' ') game->saveBestNetwork("weights.txt");

#ifdef GRAPHICS
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
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
      breedWithBest = true;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
      breedWithBest = false;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
      game->saveBestNetwork("weights.txt");
    }

    speed = std::max(0, speed);

    if(speed > 0)
      window->clear(sf::Color::Black);
#endif
    clear();
    game->update(speed);
    refresh();

#ifdef GRAPHICS
    if(speed > 0)
      window->display();
#endif

    mvprintw(3, 0, "speed = %d", speed);
    mvprintw(3, 15, "breedWithbest = %d", breedWithBest);

#ifdef GRAPHICS
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
#endif
  }
  endwin();
}
