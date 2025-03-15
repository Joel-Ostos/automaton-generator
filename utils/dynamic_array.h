#ifndef DYNAMIC_H
#define DYNAMIC_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#define INIT_SIZE 	30
#define GROW_FACTOR 	2

typedef struct Dynamic_array Dynamic_array;

struct Dynamic_array {
  size_t cap;
  size_t used;
};

#define da_cast(x) ((Dynamic_array*) x - 1)
#define da_put(x, v) (da_check(da_cast(x)) ? (x)[da_cast(x)->used] = v, da_cast(x)->used++ : (da_grow(x, sizeof(x[0])), (x)[da_cast(x)->used] = v, da_cast(x)->used++))
#define da_append(x, v) (!(x) ?  x = da_init(x, sizeof(*x)), da_put(x, v) : da_put(x,v))
#define da_pop(x)   (!(x) ? x = da_init(x, sizeof(*x)): da_delete(x))
#define da_delete(x)  (da_cast(x)->used == 0 ? (x)[da_cast(x)->used]: (da_cast(x)->used--, (x)[da_cast(x)->used]))
#define da_size(x)  (!(x) ? 0 : da_cast(x)->used)

bool da_check(Dynamic_array* x) 
{
  return !(x->used == (x->cap-1));
}

void* da_init(void* x, size_t elem_size) 
{
  //printf("\nVa a llamar\n");
  void* b = malloc(INIT_SIZE * elem_size + sizeof(Dynamic_array));
  //printf("\nFUNCA\n");
  ((Dynamic_array*)b)->cap = INIT_SIZE;
  ((Dynamic_array*)b)->used = 0;
  b = (char*) b + sizeof(Dynamic_array);
  assert(b);
  return b;
}

void* da_grow(void* x, size_t elem_size) 
{
  void* b = realloc(da_cast(x), ((Dynamic_array*)b)->cap * GROW_FACTOR * elem_size + sizeof(Dynamic_array));
  ((Dynamic_array*)b)->cap = INIT_SIZE * GROW_FACTOR;
  b = (char*) b + sizeof(Dynamic_array);
  assert(b);
  return b;
}

#endif
