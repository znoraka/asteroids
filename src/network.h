#pragma once

#include <vector>
#include <numeric>
#include <fstream>
#include <iostream>

#include "neuron.h"

class Network {
public:
  Network(std::vector<int> dimensions);

  void compute();
  void mutate(float mutationRate = 0.1f);
  void breed(Network *p1, Network *p2, float mutationRate = 0.05f);
  void randomValues();
  void save(std::string filename);
  
  void setInputs(std::vector<float> inputs);
  std::vector<Neuron *> getOutputs() const;
  void copyNetworkValues(Network *n);

  std::vector<std::vector<Neuron*> > layers;

private:
};


Network::Network(std::vector<int> dimensions) {    
  for(auto i : dimensions) {
    std::vector<Neuron *> v;
    for (int j = 0; j < i; j++) {
      v.push_back(new Neuron());
    }
    layers.push_back(v);
  }

  for (int i = 1; i < layers.size(); i++) {
    std::vector<Neuron*> currentLayer = layers[i];
    std::vector<Neuron*> previousLayer = layers[i - 1];

    for(auto c : currentLayer) {
      for(auto p : previousLayer) {
	c->connect(p);
      }
    }
  }
}

void Network::setInputs(std::vector<float> inputs) {
  for (int i = 0; i < inputs.size(); i++) {
    layers[0][i]->setValue(inputs[i]);
  }
}

void Network::compute() {
  for (int i = 1; i < layers.size(); i++) {
    for (int j = 0; j < layers[i].size(); j++) {
      this->layers[i][j]->activate();
    }
  }

}

std::vector<Neuron *> Network::getOutputs() const {
  return layers[layers.size() - 1];
}

void Network::mutate(float mutationRate) {
  for (int i = 1; i < layers.size(); i++) {
    for (int j = 0; j < layers[i].size(); j++) {
      this->layers[i][j]->mutate(mutationRate);
    }
  }
}

void Network::copyNetworkValues(Network *n) {
  for (int i = 0; i < layers.size(); i++) {
    for (int j = 0; j < layers[i].size(); j++) {
      this->layers[i][j]->weights = n->layers[i][j]->weights;
      this->layers[i][j]->weight = n->layers[i][j]->weight;
    }
  }
}

void Network::breed(Network *p1, Network *p2, float mutationRate) {
  for (int i = 1; i < layers.size(); i++) {
    for (int j = 0; j < layers[i].size(); j++) {
      for (int k = 0; k < layers[i][j]->weights.size(); k++) {
	this->layers[i][j]->weights[k] = (std::rand() % 100 > 40) ? p1->layers[i][j]->weights[k] : p2->layers[i][j]->weights[k];
      }
      if((std::rand() % 100) * 0.01f < mutationRate) {
	this->layers[i][j]->mutate(mutationRate);
      }
    }
  }
}

void Network::randomValues() {
  for (int i = 1; i < layers.size(); i++) {
    for (int j = 0; j < layers[i].size(); j++) {
      this->layers[i][j]->randomValues();
    }
  }
}

void Network::save(std::string filename) {
  std::ofstream file;
  file.open (filename);

  for (int i = 1; i < layers.size(); i++) {
    for (int j = 0; j < layers[i].size(); j++) {
      this->layers[i][j]->save(file);
    }
  }
  file.close();
}
