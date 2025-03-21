#ifndef AUTOMATA_H
#define AUTOMATA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../utils/regex.h"

typedef struct automata automata;
typedef struct State State;
typedef struct Edge Edge;

struct Edge {
  int 		to;
  char 		label;
};

struct State {
  bool		accept;
  Edge* 	edges;
};

struct automata {
  State* 	list;
};

#endif
