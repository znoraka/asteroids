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

The file "stats.txt" gives statistics on the current networks, you can use gnuplot to view it :

```sh
plot 'stats.txt' using 0:1 with lines title "generation max", 'stats.txt' using 0:2 with lines title "selected best", 'stats.txt' using 0:3 with lines title "generation avg"
```
