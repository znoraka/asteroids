#pragma once

#include <vector>
#include <numeric>
#include <fstream>

std::function<float(float)> sigmoid = [](float f) {
  return 1 / (1 + exp(-f));
};

std::function<float(float)> htan = [](float f) {
  return (1 - exp(-2 * f)) / (1 + exp(2 * f));
};

class Neuron {
public:
  Neuron(float weight);
  Neuron();
  void connect(Neuron *n);
  float getOutput() const;
  void activate();
  void setValue(float f);
  void mutate(float mutationRate);
  void randomValues();
  void save(std::ostream& stream);
  std::vector<float> weights;
  float weight;
  
private:
  std::vector<Neuron*> inputs;
  float output;
};

Neuron::Neuron() {
  this->weight = ((std::rand() % 100)) * 0.01;
}

Neuron::Neuron(float weight) : weight(weight) {
  
}

/**
 * adds input to the neuron
 */
void Neuron::connect(Neuron *n) {
  inputs.push_back(n);
  weights.push_back(this->weight = ((std::rand() % 200) - 100) * 0.01);
}

float Neuron::getOutput() const {
  return output;
}

void Neuron::activate() {
  float f = 0;
  for (int i = 0; i < inputs.size(); i++) {
    f += inputs[i]->getOutput() * weights[i];
  }  
  output = sigmoid(f);
  // if(output < weight) output = 0;
}

void Neuron::mutate(float mutationRate) {
  for(auto& i : weights) {
    if((std::rand() % 1000) * 0.001f < mutationRate) {
      i = ((std::rand() % 2000) - 1000) * 0.002;
    }
  }
}

void Neuron::randomValues() {
  for(auto& i : weights) {
    i = ((std::rand() % 2000) - 1000) * 0.001f;
  }
}

void Neuron::setValue(float f) {
  this->output = f;
}

void Neuron::save(std::ostream& stream) {
  for(auto i : weights) {
    stream << i << " ";
  }
  stream << "\n";
}
