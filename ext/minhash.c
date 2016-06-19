#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "murmurhash.h"
#include "bitvector.h"

#define THREADS 4

BitVector *
to_minhash_c(char *words[], uint32_t num, uint32_t *seeds, uint32_t k, uint32_t b) {
  BitVector *new = alloc(k*b);
  uint32_t i, j, l, hash, hash_min;
  for(i=0; i<k; i++) {
    hash_min = UINT32_MAX;
    for(j=0; j<num; j++) {
      hash = murmurhash(words[j], strlen(words[j]), seeds[i]);
      hash_min = (hash_min < hash) ? hash_min : hash;
    }
    for(l=0; l<b; l++) {
      hash = hash_min & 0x01;
      if(hash == 1)
        set_bit(new, (i*b)+l);
      hash = hash >> 1;
    }
  }
  return new;
}

float
jaccard_c(const BitVector *x, const BitVector *y, uint32_t k, uint32_t b) {
  uint32_t i, j, n = 0;
  bool same;
  for(i=0; i<k; i++) {
    same = true;
    for(j=0; j<b; j++) {
      if(get_bit(x, (i*b)+j) != get_bit(y, (i*b)+j)) {
        same = false;
        break;
      }
    }
    if(same) n++;
  }
  float p = (float)n/(float)n;
  float bi = (float)pow(2.0, -1.0*(double)b);
  return (p-bi)/(1.0-bi);
}
