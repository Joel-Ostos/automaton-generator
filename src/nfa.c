#include "nfa.h"

int main() 
{
  regex reg;
  NFA nfa;
  reg = "a?(a+b)?b";
  nfa = process_regex(reg);

  reg = "a+b";
  nfa = process_regex(reg);

  reg = "a?b";
  nfa = process_regex(reg);

  reg = "a*";
  nfa = process_regex(reg);
  
  reg = "a?(b?a*)";
  nfa = process_regex(reg);
  
  reg = "a?(b?a*?(b?b)*?b)*?a";
  nfa = process_regex(reg);

  reg = "(0+(1?(0?1*?(0?0)*?0)*?1)*)*";
  nfa = process_regex(reg);
}
