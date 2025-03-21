#ifndef NFA_H
#define NFA_H

#include "nfa_lambda.h"

#define Nfa automata

typedef struct Transition Transition;

struct Transition {
  int from;
  char character;
  int* to;
};

bool find_symbol(char* symbols, char x) 
{
  for (size_t i = 0; i < da_size(symbols); i++)
    if (symbols[i] == x) return 1;
  return 0;
}

char* get_symbols(NfaLambda* nfa_lambda) 
{
  char* symbols = {0};
  for (size_t i = 0; i < da_size(nfa_lambda->list); i++) {
    for (size_t j = 0; j < da_size(nfa_lambda->list[i].edges); j++) {
      Edge actual = nfa_lambda->list[i].edges[j];
      if (actual.label == '&') continue;
      if (!find_symbol(symbols, actual.label)) {
	da_append(symbols, actual.label);
      }
    }
  }
  return symbols;
}


void set_false(bool* arr, size_t size) 
{
  for (size_t i = 0; i < size; i++) {
    arr[i] = false;
  }
}

int* bfs(NfaLambda* nfa, int* elements, int from) 
{
  int* q = {0};
  queue_push(q, from);

  bool visited[da_size(nfa->list)];
  set_false(visited, da_size(nfa->list));

  while (!queue_empty(q)) {
    int actual = queue_pop(q);
    if (visited[actual]) continue;
    for (size_t j = 0; j < da_size(nfa->list[actual].edges); j++) {
      Edge edge = nfa->list[actual].edges[j];
      if (edge.label != '&') continue;
      queue_push(q, edge.to);
      da_append(elements, edge.to);
    }
    visited[actual] = true;
  }

  return elements;
}

int** get_lambda_transitions(NfaLambda* nfa) 
{
  int** result = {0};

  for (size_t i = 0; i < da_size(nfa->list); i++) {
    int* elements = {0};
    da_append(elements, i);
    if (da_size(nfa->list[i].edges) == 0) {
      da_append(result, elements);
      continue;
    }

    bfs(nfa, elements, (int)i);

    da_append(result, elements);
  }
  return result;
}

Transition* get_new_transitions(NfaLambda* nfa, int*** l_transitions, char** symbols) 
{
  Transition* result = {0};

  for (size_t i = 0; i < da_size(*l_transitions); i++) {
    int* to = (*l_transitions)[i];

    for (size_t  j = 0; j < da_size(to); j++) {
      for (size_t k = 0; k < da_size(*symbols); k++) {
	Transition actual;
	actual.from = i;
	actual.to = NULL;
	actual.character = (*symbols)[k];
	for (size_t l = 0; l < da_size(nfa->list[to[j]].edges); l++) {
	  Edge actual_edge = nfa->list[to[j]].edges[l];
	  if (actual_edge.label != actual.character) continue;
	  da_append(actual.to, actual_edge.to);
	  bfs(nfa, actual.to, actual_edge.to);
	}
	da_append(result, actual);
      }
    }
  }

  return result;
}

void build_nfa(Nfa* nfa, NfaLambda* nfa_lambda, Transition* transitions) 
{
  for (size_t i = 0; i < da_size(nfa_lambda->list); i++) {
    State state = {false, NULL};
    da_append(nfa->list, state);
  }

  for (size_t i = 0; i < da_size(transitions); i++) {
    Transition actual = transitions[i];
    for (size_t j = 0; j < da_size(actual.to); j++) {
      Edge edge = {actual.to[j], actual.character};
      da_append(nfa->list[actual.from].edges, edge);
    }
  }
}

Nfa minimize(NfaLambda* nfa_lambda)
{
  Nfa result = {0};
  char* symbols = get_symbols(nfa_lambda);
  int** lambda_transitions = get_lambda_transitions(nfa_lambda);

  for (size_t i = 0; i < da_size(lambda_transitions); i++) {
    printf("%zu = {%zu", i, i);
    for (size_t j = 1; j < da_size(lambda_transitions[i]); j++) {
      printf(" , %d", lambda_transitions[i][j]);
    }
    printf("}\n");
  }

  Transition* new_transitions = get_new_transitions(nfa_lambda, &lambda_transitions, &symbols);

  for (size_t i = 0; i < da_size(new_transitions); i++) {
    Transition actual = new_transitions[i];
    if (da_size(actual.to) == 0) continue;
    printf("From %d:\n", actual.from);
    for (size_t j = 0; j < da_size(actual.to); j++) {
      printf("\t%c -> %d\n", actual.character, actual.to[j]);
    }
  }

  build_nfa(&result, nfa_lambda, new_transitions);
  print_nfa(&result);
  return result;
}

#endif
