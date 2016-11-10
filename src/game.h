#pragma once

#include <algorithm>
#include <iostream>
#include <omp.h>

#define SHIP_SIZE 10
#define SHIP_COUNT 15

#define ASTEROID_COUNT 20
#define ASTEROID_SIZE 30

std::vector<float> sins;
std::vector<float> coss;

class Ship {
public:
  Ship();
  void init(float x, float y);
  void update();
  void draw();
  
  float x;
  float y;
  Network *network;
  bool alive;
  float speed = 8.0f;
  int score = 0;
};

class Asteroid {
public:
  Asteroid();
  void update();
  void init();
  void draw();

  float x;
  float y;

  int size;

  float xDir;
  float yDir;

  float startX;
  float startY;

  float speed = 13.2f;
};

class Game {
private:
public:
  int bestScore = 0;
  Network *bestNetwork;
  int score = 0;

  void update(int delta);
  void saveBestNetwork(std::string filename);
  void loadNetwork(std::string filename);
  Game();

  std::vector<Asteroid*> asteroids;
  std::vector<Ship*> ships;

  int aliveShips = 0;
  int generation = 0;

  int frames = 0;

  void createNetworks(std::vector<int> dimensions);

};

static Game *game = new Game();

float raycast(std::vector<Asteroid*> asteroids, Ship *ship, int index) {
  float x1 = 1;
  float y1 = 0;

  x1 = SHIP_SIZE * coss[index];
  y1 = SHIP_SIZE * sins[index];

  float x2 = ship->x;
  float y2 = ship->y;

  for(;;) {
    if(x2 > 800 || x2 < 0 || y2 > 600 || y2 < 0) {
      return sqrt((ship->x - x2) * (ship->x - x2) + (ship->y - y2) * (ship->y - y2));
    }

    for(auto i : asteroids) {
      if((i->x - x2) * (i->x - x2) + (i->y - y2) * (i->y - y2) < (i->size + SHIP_SIZE) * (i->size + SHIP_SIZE)) {
	return sqrt((ship->x - x2) * (ship->x - x2) + (ship->y - y2) * (ship->y - y2));
      }
    }
    x2 += x1;
    y2 += y1;
  }
}


Game::Game() {
  std::srand(std::time(NULL));

  for (int i = 0; i < SHIP_COUNT; i++) {
    Ship *s = new Ship();
    s->init(400, 300);
    ships.push_back(s);
  } 
  aliveShips = ships.size();

  for (int i = 0; i < ASTEROID_COUNT; i++) {
    asteroids.push_back(new Asteroid());
  }
}

void Game::update(int delta) {
  score++;
  frames++;
  for(auto i : asteroids) {
    i->update();
    if(delta > 0) i->draw();
  }

  for(auto i : ships) {
    if(i->alive) {
      i->update();
      if(delta > 0) i->draw();

      int div = ships[0]->network->layers[0].size();
      std::vector<float> inputs(div, 0);
#pragma omp parallel for
      for (int j = 0; j < div; j++) {
	inputs[j] = raycast(asteroids, i, j) * 0.001f;
      }

#ifdef GRAPHICS
      // int k = 0;
      // for(auto j : inputs) {
      // 	sf::Vertex line[] =
      // 	  {
      // 	    sf::Vertex(sf::Vector2f(i->x, i->y), sf::Color(255, 0, 0)),
      // 	    sf::Vertex(sf::Vector2f(i->x + j * 1000 * coss[k], i->y + j * 1000 * sins[k]), sf::Color(255, 0, 0))
      // 	  };
      // 	  window->draw(line, 2, sf::Lines);
      // 	k++;
      // }
#endif
      
      i->network->setInputs({inputs});
    }
  }

  if(aliveShips <= 0) {
    generation++;

    aliveShips = ships.size();
    int bestScore = 0;
    int index = 0;

    for(auto i : asteroids) {
      i->init();
    }

    if(this->score > this->bestScore) {
      this->bestScore = score;
      this->bestNetwork->copyNetworkValues(Network::bestNetwork);
    }
    score = 0;

    for (int i = 0; i < ships.size(); i++) {
      ships[i]->alive = true;
      ships[i]->score = 0;
      ships[i]->init(400, 300);
    }

    Network::breed(0.1f);
  }

  mvprintw(0, 0, "bestscore = %d  score = %d", this->bestScore, score);
  mvprintw(1, 0, "alive = %d", aliveShips);
  mvprintw(2, 0, "generation = %d", generation);
}

void Game::createNetworks(std::vector<int> dimensions) {
  Network::generateNetworks(ships.size(), dimensions);
  
  for (int i = 0; i < ships.size(); i++) {
    ships[i]->network = Network::networks[i];
  }
  this->bestNetwork = new Network(dimensions);

  float n = dimensions[0];
  coss.resize(n);
  sins.resize(n);
  
  for (int i = 0; i < n; i++) {
    coss[i] = cos((i / n) * 2 * M_PI);
    sins[i] = sin((i / n) * 2 * M_PI);
  }
  
}

Ship::Ship() {
  
}

void Ship::init(float x, float y) {
  this->x = x;
  this->y = y;
  this->score = 0;
  this->alive = true;
}

void Ship::draw() {
#ifdef GRAPHICS
  sf::CircleShape shape(SHIP_SIZE);
  shape.setOrigin(SHIP_SIZE * 0.5, SHIP_SIZE * 0.5);
  shape.setFillColor(sf::Color(100, 250, 50));
  shape.setPosition(x, y);

  window->draw(shape);
#endif
}

void Ship::update() {
  if(alive) {
    score++;
    this->network->score++;

    if(x < 0 || x > 800 || y < 0 || y > 600) {
      alive = false;
      game->aliveShips--;
    }

    if(x < 0) x = 800;
    if(x > 800) x = 0;
    if(y < 0) y = 600;
    if(y > 600) y = 0;

    for(auto i : game->asteroids) {
      float distance = (i->x - x) * (i->x - x) + (i->y - y) * (i->y - y);

      if(distance < (SHIP_SIZE + i->size) * (SHIP_SIZE + i->size))  {
	alive = false;
	game->aliveShips--;
      }
    }

    network->compute();

    x += (network->getOutputs()[0]->getOutput() - 0.5f) * speed;
    y += (network->getOutputs()[1]->getOutput() - 0.5f) * speed;
  }
}

Asteroid::Asteroid() {
  init();
}

void Asteroid::update() {
  x += xDir * speed;
  y += yDir * speed;
  
  if(x < 0) x = 800;
  if(x > 800) x = 0;
  if(y < 0) y = 600;
  if(y > 600) y = 0;
}

void Asteroid::init() {
  x = startX;
  y = startY;

  size = 5 + std::rand() % ASTEROID_SIZE;

  while((x - 400) * (x - 400) + (y - 300) * (y - 300) < 200*200) {
    x = std::rand() % 800;
    y = std::rand() % 600;
  }

  startX = x;
  startY = y;
  
  xDir = (std::rand() % 100) * 0.01 - 0.5;
  yDir = (std::rand() % 100) * 0.01 - 0.5;

  speed = std::rand() % 10;
}

void Asteroid::draw() {
#ifdef GRAPHICS
  sf::CircleShape shape(size);
  shape.setFillColor(sf::Color(200, 50, 50));
  shape.setPosition(x - size, y - size);

  window->draw(shape);
#endif
}

void Game::saveBestNetwork(std::string filename) {
  bestNetwork->save(filename);
}

void Game::loadNetwork(std::string filename) {
  bestNetwork->load(filename);
  for(auto i : ships) {
    i->network->copyNetworkValues(bestNetwork);
    i->network->compute();
  }
}
