#ifndef NFA_H
#define NFA_H
#include "nfa_lambda.h"

typedef struct Nfa Nfa;
struct Nfa {
  State* list;
};

Nfa minimize(NfaLambda* nfa_lambda)
{
  Nfa result = {0};
  return result;
}

#endif
