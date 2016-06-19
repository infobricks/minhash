#ifndef BITVECTOR
#define BITVECTOR

#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define UBYTES (sizeof(uint32_t))
#define UBITS (UBYTES * CHAR_BIT)
#define bitmask(bit) (1 << ((bit) % UBITS))
#define get_container_size(bits) (bits <= 0 ? 0 : (((bits)-1)/UBITS+1))

/* BitVector structure */
typedef struct bitvector {
  long bits;                  /* number of bits */
  uint32_t num_container; /* number of vector size */
  uint32_t *container;    /* bit container as vector */
} BitVector;

/* Create new BitVector object */
BitVector *alloc(long size) {
  BitVector *vector = (BitVector *)malloc(sizeof(BitVector));
  vector->bits = size;
  if(size>=0) {
    vector->num_container = get_container_size(size);
    vector->container = (uint32_t *)calloc(vector->num_container, UBYTES);
  } else {
    vector->num_container = 0;
    vector->container = NULL;
  }
  return vector;
}

/* Delete a BitVector object */
void dealloc(BitVector *vector) {
  free(vector->container);
  memset(vector, 0, sizeof(BitVector));
}

/* Get bitvector index(to support suffix access) */
long get_index(const BitVector *vector, long index) {
  if(index < 0) index += vector->bits;
  if(index >= vector->bits) return -1;
  return index;
}

/* Set the specific index bit to 1 */
void set_bit(BitVector *vector, long index) {
  index = get_index(vector, index);
  if(index >= 0)
    vector->container[index/UBITS] |= bitmask(index);
}

/* Set all bits to 1 */
void set_all_bits(BitVector *vector) {
  memset(vector->container, 0xff, (vector->num_container * UBYTES));
}

/* Unset the specific index bit to 0 */
void clear_bit(BitVector *vector, long index) {
  index = get_index(vector, index);
  if(index >= 0)
    vector->container[index/UBITS] &= ~bitmask(index);
}

/* Unset all bits to 0 */
void clear_all_bits(BitVector *vector) {
  memset(vector->container, 0x00, (vector->num_container * UBYTES));
}

/* Assign value to specified index */
void assign_bit(BitVector *vector, long index, bool value) {
  if(value)
    set_bit(vector, index);
  else
    clear_bit(vector, index);
}

/* Get bit of the index(true or false) */
bool get_bit(const BitVector *vector, long index) {
  index = get_index(vector, index);
  if(index < 0) return false;
  unsigned int bit = (vector->container[index/UBITS] & bitmask(index));
  if(bit>0) 
    return true;
  else
    return false;
}

/* Create new BitVector object from string data such like "0101" */
BitVector *from_string(char *str) {
  long size = strlen(str);
  BitVector *vector = alloc(size);
  /* ToDO: parse str and set bits */
  long i;
  for(i=0; i<size; i++) {
    if(str[i] != '0' && str[i] != '1') {
      str[i] = '\0';
      break;
    }
  }
  for(i=0; i<vector->bits; i++) {
    if(str[i] == '1')
      set_bit(vector, i);
    else
      clear_bit(vector, i);
  }
  return vector;
}

/* Create string data from BitVector object */
char *to_string(const BitVector *vector) {
  char *str = (char*)malloc(sizeof(char)*(vector->bits+1));
  long i;
  for(i=0; i<vector->bits; i++)
    str[i] = get_bit(vector, i) ? '1' : '0';
  str[vector->bits] = '\0';
  return str;
}

/* create intersect bit vector from two ones */
BitVector *c_intersect(const BitVector *x, const BitVector *y) {
  const BitVector *shorter = ((x->bits < y->bits) ? x : y);
  BitVector *new = alloc(shorter->bits);
  long i;
  for(i=0; i<new->num_container; i++)
    new->container[i] = x->container[i] & y->container[i];
  return new;
}

/* create union bit vector from two ones */
BitVector *c_union(const BitVector *x, const BitVector *y) {
  const BitVector *longer = ((x->bits > y->bits) ? x : y);
  const BitVector *shorter = ((longer == x) ? y : x);
  BitVector *new = alloc(longer->bits);
  long i;
  for(i=0; i<shorter->num_container-1; i++)
    new->container[i] = longer->container[i] | shorter->container[i];
  long start = ((shorter->num_container-1)*UBITS);
  for(i=start; i<shorter->bits; i++)
    assign_bit(new, i, get_bit(longer, i) | get_bit(shorter, i));
  return new;
}

/* */
BitVector *c_conjunction(const BitVector *x, const BitVector *y) {
  const BitVector *shorter = ((x->bits < y->bits) ? x : y);
  BitVector *new = alloc(shorter->bits);
  long i;
  for(i=0; i<new->bits; i++) {
    if(get_bit(x, i) == get_bit(y, i))
      set_bit(new, i);
  }
  return new;
}

/*  */
BitVector *c_exclusive(const BitVector *x, const BitVector *y) {
  const BitVector *longer = ((x->bits > y->bits) ? x : y);
  const BitVector *shorter = ((x == longer) ? y : x);
  BitVector *new = alloc(longer->bits);
  long i;
  for(i=0; i<shorter->bits; i++) {
    if(get_bit(x, i) != get_bit(y, i))
      set_bit(new, i);
  }
  return new;
}

/* concat two bit vector and generate new one */
BitVector *c_concat(const BitVector *x, const BitVector *y) {
  BitVector *new = alloc(x->bits + y->bits);
  long i;
  for(i=0; i<new->bits; i++) {
    if(i<x->bits)
      assign_bit(new, i, get_bit(x, i));
    else
      assign_bit(new, i, get_bit(y, i-(x->bits)));
  }
  return new;
}

uint64_t c_to_decimal(const BitVector *vector) {
  uint64_t value = 0;
  long i;
  for(i=0; i<vector->bits; i++)
    if(get_bit(vector, i)) value += (uint64_t)pow(2, i);
  return value;
}

BitVector *c_slice(const BitVector *vector, long from, long to) {
  long size = to - from + 1;
  BitVector *new = alloc(size);
  long i;
  for(i=0; i<size; i++)
    if(get_bit(vector, i+from)) set_bit(new, i);
  return new;
}

uint64_t c_count(const BitVector *vector) {
  uint64_t count = 0;
  long i;
  for(i=0; i<vector->bits; i++)
    if(get_bit(vector, i)) count++;
  return count;
}

#endif
