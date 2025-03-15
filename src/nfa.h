#ifndef NFA_H
#define NFA_H

#include <stdio.h>
#include "../utils/regex.h"

typedef struct State 	State;
typedef struct NFA 	NFA;
typedef struct Edge 	Edge;

struct Edge {
  int 		to;
  char 		label;
};

struct State {
  bool 		accept;
  Edge* 	edges;
};

struct NFA {
  State* 	list;
};

NFA generate_nfa()
{
  NFA tmp;
  tmp.list = NULL;
  return tmp;
}

void make_copy(NFA* dst, NFA* src, size_t idx, int jump, bool flag) 
{
  for (size_t i = idx; i < da_size(src->list); i++) {
    State actual = src->list[i];
    for (size_t j = 0; j < da_size(actual.edges); j++) {
      if (!flag) break;
      actual.edges[j].to += jump;
    }
    actual.accept = false;
    da_append(dst->list, actual);
  }
}

NFA make_single(char eaten) 
{
  NFA result = {NULL};

  State a = {false, NULL};

  State b = {true, NULL}; 

  Edge edge = {1, eaten};

  da_append(a.edges, edge);
  da_append(result.list, a);
  da_append(result.list, b);
  
  return result;
}

NFA make_union(NFA machine_a, NFA machine_b) 
{
  NFA result = {0};
  State init = {false, NULL};
  State end = {true, NULL};

  Edge connect_to_a = {1, '&'};
  Edge connect_to_b = {da_size(machine_a.list)+1, '&'};

  da_append(init.edges, connect_to_a);
  da_append(init.edges, connect_to_b);
  da_append(result.list, init);


  size_t jump = da_size(result.list);
  size_t jump_b = da_size(machine_b.list);
  Edge last = {(int)(jump + jump_b),'&'};

  da_append(result.list[jump-1].edges, last);
  make_copy(&result, &machine_a, 0, 1, true);
  make_copy(&result, &machine_b, 0, jump, true);


  da_append(result.list, end);
  last.to = da_size(result.list)-1;
  last.label = '&';
  da_append(result.list[da_size(result.list)-2].edges, last);

  return result;
}

NFA make_concat(NFA machine_a, NFA machine_b) 
{
  NFA result = {0};
  
  // Agregamos todo $a al NFA final, aumentando en 1 cada arista.
  make_copy(&result, &machine_a, 0, 1, false);

  size_t jump = da_size(result.list);
  result.list[jump-1] = machine_b.list[0];

  for (size_t j = 0; j < da_size(result.list[jump-1].edges); j++) {
    result.list[jump-1].edges[j].to = result.list[jump-1].edges[j].to + jump - 1;
  }

  make_copy(&result, &machine_b, 1, jump-1, false);

  jump = da_size(result.list);

  result.list[jump-1].accept = true;

  return result;
}

NFA make_kleene(NFA machine_a) 
{
  NFA result = {0};
  State init = {false, NULL};
  State end = {true, NULL};
  Edge to_a = {1, '&'};

  da_append(result.list, init);
  da_append(result.list[0].edges, to_a);

  to_a.to = 0;
  da_append(machine_a.list[da_size(machine_a.list)-1].edges, to_a);

  to_a.to = da_size(machine_a.list)+1;

  da_append(result.list[0].edges, to_a);
  //da_append(machine_a.list[da_size(machine_a.list)-1].edges, to_a);

  make_copy(&result, &machine_a, 0, 1, true);

  da_append(result.list, end);

  to_a.to = 0;
  da_append(result.list[da_size(machine_a.list)+1].edges, to_a);
  to_a.to = da_size(machine_a.list)+1;
  da_append(result.list[da_size(machine_a.list)].edges, to_a);

  return result;
}

void print_nfa(NFA* nfa) 
{
  printf("\n----------------------------------------------------\n");
  printf("Tamaño NFA: %zu\n", da_size(nfa->list));
  for (int i = 0; i < da_size(nfa->list); i++) {
    printf("State %d ¿accept? %s to:\n", i, nfa->list[i].accept ? "true" : "false");
    for (int j = 0; j < da_size(nfa->list[i].edges); j++) {
      printf("\t\t\t  -> %d, label -> %c\n", nfa->list[i].edges[j].to, nfa->list[i].edges[j].label);
    }
  }
  printf("\n----------------------------------------------------\n");
}

NFA process_regex(regex reg) 
{
  regex postfix_regex = shunting_postfix(reg);
  printf("Regex: %s\nPostfix regex: %s\n", reg,postfix_regex);
  NFA* machines = {0};
  NFA result = generate_nfa();

  for (int i = 0; i < strlen(postfix_regex); i++) {
    //if (i == 18) break;
    if (isalnum(postfix_regex[i])) {
      NFA temp = make_single(postfix_regex[i]);
      if (i) assert(machines != NULL);
      stack_push(machines, temp);
      continue;
    }

    switch (postfix_regex[i]) {
      case '+': {
		  NFA b = stack_pop(machines);
		  NFA a = stack_pop(machines);
		  NFA temp = make_union(a,b);
		  stack_push(machines, temp);
		  break;
		}
      case '*': {
		  NFA a = stack_pop(machines);
		  NFA temp = make_kleene(a);
		  stack_push(machines, temp);
		  break;
		}
      case '?' : {
		   NFA b = stack_pop(machines);
		   NFA a = stack_pop(machines);
		   NFA temp = make_concat(a,b);
		   stack_push(machines, temp);
		   break;
		 }
    }
  }

  result = stack_pop(machines);
  print_nfa(&result);

  return result;
}

#endif
