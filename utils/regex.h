#ifndef REGEX_H
#define REGEX_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"
#include "dynamic_array.h"
#include "stack.h"

enum  op {CL_PAR, UNION, CONCAT, KLEENE, OP_PAR, NONE};

typedef char* regex;

enum op getop(char x) 
{
  switch(x) {
    case ')': return CL_PAR;
    case '+': return UNION;
    case '?': return CONCAT;
    case '*': return KLEENE;
    case '(': return OP_PAR;
  }
  return NONE;
}

void reordenate_stack(char x, char** s, char** q) 
{
  if ((stack_size(*s)) == 0) {
    stack_push(*s, x);
    return;
  }

  enum op top = getop(stack_top(*s));
  enum op oper = getop(x);

  if (oper == OP_PAR) {
    stack_push(*s, x);
    return;
  }

  if (oper == CL_PAR) {
    while (top != OP_PAR && (stack_size(*s)) > 0) {
      char operand = stack_pop(*s);
      queue_push(*q, operand);
      operand = stack_top(*s);
      top = getop(operand);
    }
    if (top == OP_PAR) {
      stack_pop(*s);
      return;
    }
    printf("Invalid regex sequence");
    exit(1);
  }

  while (oper < top && (stack_size(*s)) > 0) {
    if (top == OP_PAR) {
      stack_push(*s, x);
      return;
    };
    char n = stack_pop(*s);
    queue_push(*q, n);
    top = getop(stack_top(*s));
  }
  stack_push(*s, x);
}

regex shunting_postfix(const regex src) 
{
  regex output = {0};
  char* opts = {0};
  char* characters = {0};
  size_t sz = strlen(src);

  for (size_t i = 0; i < sz; i++) {
    if (isalnum(src[i])) queue_push(characters, src[i]);
    else if (getop(src[i]) != NONE) reordenate_stack(src[i], &opts, &characters);
  }

  while (!(queue_empty(characters))) {
    char character = queue_pop(characters);
    da_append(output, character);
  }

  while (!(stack_empty(opts))) {
    char opt = stack_pop(opts);
    da_append(output, opt);
  }

  return output;
}


regex shunting_prefix(regex* src) 
{
  regex output = (regex) "";
  char* opts = {0};
  char* characters = {0};
  return output;
}

#endif
