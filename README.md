# README

Compilation (Linux) :

```sh
$ cmake .
$ make
```
You need the SFML development packages.

Usage :

```sh
$ bin/ml <trained network> <inputs number> <neuron number>... <outputs number>
```
Note: there must be at least 2 outputs (only 2 will be used).

Examples : 

```sh
$ bin/ml "" 16 10 2
$ bin/ml "weights_16_10_2" 16 10 2
$ bin/ml "" 64 35 35 2
```