#pragma once

#include <algorithm>

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
  float speed = 17.5f;
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

  float xDir;
  float yDir;

  float speed = 13.2f;
};

class Game {
private:
public:
  int bestScore = 0;
  Network *bestNetwork;
  int score = 0;

  std::ofstream stats;

  void update(int delta);
  void saveBestNetwork(std::string filename);
  Game();

  std::vector<Asteroid*> asteroids;
  std::vector<Ship*> ships;

  int aliveShips = 0;
  int generation = 0;

  void createNetworks(std::vector<int> dimensions);

};

static Game *game = new Game();

Game::Game() {
  for (int i = 0; i < 100; i++) {
    Ship *s = new Ship();
    s->init(400, 300);
    ships.push_back(s);
  } 
  aliveShips = ships.size();

  for (int i = 0; i < 40; i++) {
    asteroids.push_back(new Asteroid());
  }
  stats.open("stats.txt");
}

void Game::update(int delta) {
  score++;
  for(auto i : asteroids) {
    i->update();
    if(delta > 0) i->draw();
  }
  
  for(auto i : ships) {
    i->update();
    if(delta > 0 && i->alive) i->draw();

    std::vector<std::pair<float, float> > asteroidsData;

    for(auto j : asteroids) {
      float n = 800 * 800;
      float distance = (i->x - j->x) * (i->x - j->x) + (i->y - j->y) * (i->y - j->y);
      // distance /= n;
      distance = sqrt(distance);
      float angle = atan2(j->y - i->y, j->x - i->x) * 57.2958 + 180;

      asteroidsData.push_back({distance, angle});
    }


    int div = ships[0]->network->layers[0].size();

    std::vector<float> inputs(div, 0);

    for(auto j : asteroidsData) {
      int index = j.second / (360 / div);

      // mvprintw(1, 20, "%f", j.second);
      // mvprintw(2, 20, "%d", index);
      
      if(inputs[index] < j.first) {
	inputs[index] = j.first;
      }
    }

    // inputs[inputs.size() - 2] = i->x;
    // inputs[inputs.size() - 1] = i->y;

    for (int j = 0; j < inputs.size(); j++) {
      mvprintw(4 + j, 0, "%f", inputs[j]);

    }

    i->network->setInputs({inputs});
  }

  if(aliveShips <= 0) {
    score = 0;
    generation++;

    aliveShips = ships.size();
    int bestScore = 0;
    int index = 0;

    for(auto i : asteroids) {
      i->init();
    }

    std::sort(ships.begin(), ships.end(), [](Ship *s1, Ship *s2) {
	return s1->score > s2->score;
      });

    if(ships[0]->score > this->bestScore) {
      this->bestScore = ships[0]->score;
      this->bestNetwork->copyNetworkValues(ships[0]->network);
    }

    mvprintw(17, 0, "bestScore1 = %d", ships[0]->score);
    mvprintw(18, 0, "bestScore2 = %d", ships[1]->score);

    float sum = 0;
    for (int i = 0; i < ships.size(); i++) {
      if(ships[i]->score != this->bestScore)
	sum += ships[i]->score;
    }

    stats << ships[0]->score << " " << this->bestScore << " " << sum / (float) (ships.size() - 1) << "\n";
    stats.flush();


    for (int i = 0; i < ships.size(); i++) {
      ships[i]->alive = true;
      sum += ships[i]->score;
      ships[i]->score = 0;
      ships[i]->init(400, 300);
    }

    // for (int i = 2; i < ships.size(); i++) {
    //   ships[i]->network->breed(ships[0]->network, ships[1]->network, 0.05f);
    //   if(i % 4 == 0) {
    // 	ships[i]->network->randomValues();
    //   }
    // }
    
    for (int i = 1; i < ships.size(); i++) {
      ships[i]->network->breed(bestNetwork, ships[0]->network, 0.05f);
      if(i % 4 == 0) {
    	ships[i]->network->randomValues();
      }
    }
    ships[ships.size() - 1]->network->copyNetworkValues(bestNetwork);
  }

  mvprintw(0, 0, "bestscore = %d  score = %d", this->bestScore, score);
  mvprintw(1, 0, "alive = %d", aliveShips);
  mvprintw(2, 0, "generation = %d", generation);
}

void Game::createNetworks(std::vector<int> dimensions) {
  for (int i = 0; i < ships.size(); i++) {
    ships[i]->network = new Network(dimensions);
    ships[i]->network->compute();
  }
  this->bestNetwork = new Network(dimensions);
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
  sf::CircleShape shape(10);
  shape.setOrigin(5, 5);
  shape.setFillColor(sf::Color(100, 250, 50));
  shape.setPosition(x, y);

  window->draw(shape);
  // mvprintw(y, x, "H");
}

void Ship::update() {
  if(alive) {
    score++;

    if(x < 0 || x > 800 || y < 0 || y > 600) {
      alive = false;
      game->aliveShips--;
    }

    // if(x < 0) x = 800;
    // if(x > 800) x = 0;
    // if(y < 0) y = 600;
    // if(y > 600) y = 0;

    for(auto i : game->asteroids) {
      float distance = (i->x - x) * (i->x - x) + (i->y - y) * (i->y - y);

      if(distance < 25*25) {
	alive = false;
	game->aliveShips--;
      }
    }

    network->compute();


    // if(network->getOutputs()[0]->getOutput() > 0.5) {
    //   x += 0.1f * network->getOutputs()[2]->getOutput() * speed;
    // } else {
    //   x -= 0.1f * network->getOutputs()[2]->getOutput() * speed;
    // }
    // if(network->getOutputs()[1]->getOutput() > 0.5) {
    //   y += 0.1f * network->getOutputs()[2]->getOutput() * speed;
    // } else {
    //   y -= 0.1f * network->getOutputs()[2]->getOutput() * speed;
    // }

    // if(network->getOutputs()[0]->getOutput() > 0.6) {
    //   x += 0.1f * speed;
    // } else if(network->getOutputs()[0]->getOutput() < 0.4){
    //   x -= 0.1f * speed;
    // }
    // if(network->getOutputs()[1]->getOutput() > 0.6) {
    //   y += 0.1f * speed;
    // } else if(network->getOutputs()[1]->getOutput() < 0.4) {
    //   y -= 0.1f * speed;
    // }

    x += (network->getOutputs()[0]->getOutput() - 0.5f) * speed;
    y += (network->getOutputs()[1]->getOutput() - 0.5f) * speed;

    // float xDir = 3 * cos((network->getOutputs()[0]->getOutput() * 360.0) / M_PI);
    // float yDir = 3 * sin((network->getOutputs()[0]->getOutput() * 360.0) / M_PI);

    // if(network->getOutputs().size() > 1) {
    //   xDir *= network->getOutputs()[1]->getOutput() * speed * 0.1;
    //   yDir *= network->getOutputs()[1]->getOutput() * speed * 0.1;
    // }

    // x += xDir;
    // y += yDir;
    
  }
}

Asteroid::Asteroid() {
  x = std::rand() % 800;
  y = std::rand() % 600;

  xDir = (std::rand() % 100) * 0.01 - 0.5;
  yDir = (std::rand() % 100) * 0.01 - 0.5;
}

void Asteroid::update() {
  x += xDir * speed;
  y += yDir * speed;

  // if(x < -200) x = 800;
  // if(x > 1000) x = -20;
  // if(y < -100) y = 700;
  // if(y > 700) y = -100;
  if(x < 0) x = 800;
  if(x > 800) x = 0;
  if(y < 0) y = 600;
  if(y > 600) y = 0;
}

void Asteroid::init() {
  speed = std::rand() % 10;
  while((x - 400) * (x - 400) + (y - 300) * (y - 300) < 200*200) {
    x = std::rand() % 800;
    y = std::rand() % 600;
  }
}

void Asteroid::draw() {
  sf::CircleShape shape(15);
  shape.setOrigin(7.5, 7.5);
  shape.setFillColor(sf::Color(200, 50, 50));
  shape.setPosition(x, y);

  window->draw(shape);
}

void Game::saveBestNetwork(std::string filename) {
  bestNetwork->save(filename);
}
