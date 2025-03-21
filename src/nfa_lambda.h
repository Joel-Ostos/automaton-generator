#ifndef NFA_LAMBDA_H
#define NFA_LAMBDA_H

#include "automata.h"

#define NfaLambda automata

NfaLambda generate_nfa()
{
  NfaLambda tmp;
  tmp.list = NULL;
  return tmp;
}

void print_nfa(NfaLambda* nfa) 
{
  printf("\n----------------------------------------------------\n");
  printf("NfaLambda Size: %zu\n", da_size(nfa->list));
  for (int i = 0; i < da_size(nfa->list); i++) {
    printf("State %d ¿accept? %s to:\n", i, nfa->list[i].accept ? "true" : "false");
    for (int j = 0; j < da_size(nfa->list[i].edges); j++) {
      printf("\t\t\t  -> %d, label -> %c\n", nfa->list[i].edges[j].to, nfa->list[i].edges[j].label);
    }
  }
  printf("\n----------------------------------------------------\n");
}

void make_copy(NfaLambda* dst, NfaLambda* src, size_t idx, int jump, bool flag) 
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

NfaLambda make_single(char eaten) 
{
  NfaLambda result = {NULL};

  State a = {false, NULL};
  State b = {true, NULL}; 
  Edge edge = {1, eaten};

  da_append(a.edges, edge);
  da_append(result.list, a);
  da_append(result.list, b);

  return result;
}

// Make an union between two NFA-lambda machines, it follows the steps of 
// the Thompson's Construction algorithm that is:
// 1. Make an init state, join it with the first state of machine_a and
// the first of machine_b.
// 2. Connect the last state of machine_a with the last state of result
// and the last state of machine_b with the last state of result.
NfaLambda make_union(NfaLambda machine_a, NfaLambda machine_b) 
{
  NfaLambda result = {0};
  State init = {false, NULL};
  State end = {true, NULL};
  Edge connect_to_a = {1, '&'};

  // Addying a new state to the beginning of the result NFA and an edge
  // to the first element of machine_a.
  da_append(init.edges, connect_to_a);
  da_append(result.list, init);
  
  // Copying machine_a into result NFA.
  make_copy(&result, &machine_a, 0, 1, true);

  // Connecting the first state with the first state of
  // machine_b through an edge
  Edge connect_to_b = {da_size(result.list), '&'};
  da_append(init.edges, connect_to_b);
  
  //Copying machine_b into result NFA and addying the gap jump.
  make_copy(&result, &machine_b, 0, da_size(result.list), true);
  
  // Making an edge between the `old` last edge from machine_a
  // and the last state of result NFA, and making and edge 
  // between the penultimate state of result and the last one.
  da_append(result.list, end);
  Edge last = { da_size(result.list)-1, last.label = '&'};
  da_append(result.list[da_size(result.list)-2].edges, last);
  da_append(result.list[da_size(machine_a.list)].edges, last);

  return result;
}

// Again, this function follows the Thompson's Construction 
// algorithm so in this case it makes the concatenation of two
// NFA-lambda machines, it preserves the first state of machine_a
// and the last state of machine_b, and merge the last of machine_a
// and the first of machine_b
NfaLambda make_concat(NfaLambda machine_a, NfaLambda machine_b) 
{
  NfaLambda result = {0};

  // We make a copy of machine_a in result NFA Lambda, we preserve the pointer
  // to the edges of every state in machine_a
  make_copy(&result, &machine_a, 0, 1, false);

  // Make the last state of result NFA be the same with the first of machine_b.
  size_t jump = da_size(result.list);
  result.list[jump-1] = machine_b.list[0];
  
  // Modyfing the edges of the last state of result with the new gap
  // created after the copy of machine_a in result.
  for (size_t j = 0; j < da_size(result.list[jump-1].edges); j++) {
    result.list[jump-1].edges[j].to += jump - 1;
  }

  // Copying the rest of machine b in result.
  make_copy(&result, &machine_b, 1, jump-1, true);

  // Changing the bool value of the last state in result because it is in all cases,
  // an acceptation state.
  jump = da_size(result.list);
  result.list[jump-1].accept = true;

  return result;
}

// This function only uses one NFA-Lambda machine.
// Creates an init state that is connected to the first 
// state of machine_a, makes a last state that is connected
// to the first state of result NFA, create an edge between 
// the first of machine_a and the last one.
NfaLambda make_kleene(NfaLambda machine_a) 
{
  NfaLambda result = {0};
  State init = {false, NULL};
  State end = {true, NULL};
  Edge to_a = {1, '&'};

  // Addying a new state to the beginning of the result NFA and an edge
  // to the first element of machine_a.
  da_append(result.list, init);
  da_append(result.list[0].edges, to_a);

  // Addying a new edge to the last element of machine_a that points to
  // the first element of machine_a
  to_a.to = 0;
  da_append(machine_a.list[da_size(machine_a.list)-1].edges, to_a);

  // Creating an edfe to the last element of result (it doesn't exists yet)
  to_a.to = da_size(machine_a.list)+1;
  da_append(result.list[0].edges, to_a);
  
  // Copying machine_a into result NFA.
  make_copy(&result, &machine_a, 0, 1, true);
  
  // Addying the last state to result and connecting the penultimate 
  // state with the last.
  da_append(result.list, end);
  to_a.to = da_size(machine_a.list)+1;
  da_append(result.list[da_size(machine_a.list)].edges, to_a);

  return result;
}


NfaLambda process_regex(regex reg) 
{
  regex postfix_regex = shunting_postfix(reg);
  printf("Regex: %s\nPostfix regex: %s\n", reg,postfix_regex);
  NfaLambda* machines = {0};
  NfaLambda result = generate_nfa();

  for (int i = 0; i < strlen(postfix_regex); i++) {
    if (isalnum(postfix_regex[i])) {
      NfaLambda temp = make_single(postfix_regex[i]);
      if (i) assert(machines != NULL);
      stack_push(machines, temp);
      continue;
    }

    switch (postfix_regex[i]) {
      case '+': {
		  NfaLambda b = stack_pop(machines);
		  NfaLambda a = stack_pop(machines);

		  NfaLambda temp = make_union(a,b);

		  stack_push(machines, temp);
		  break;
		}
      case '*': {
		  NfaLambda a = stack_pop(machines);

		  NfaLambda temp = make_kleene(a);

		  stack_push(machines, temp);
		  break;
		}
      case '?' : {
		   NfaLambda b = stack_pop(machines);
		   NfaLambda a = stack_pop(machines);

		   NfaLambda temp = make_concat(a,b);

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
