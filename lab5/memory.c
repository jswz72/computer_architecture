#include "memory.h"

unsigned int clockX;
unsigned int numMisses;
int cache_org;

void resetClock()
{
  clockX = 0;
  numMisses = 0;
}

void printCacheOrg(int org)
{
  printf("Cache Organization: ");
  if (org == DIRECT)
    printf ("DIRECT MAPPED\n");
  else if (org == TWOWAY)
    printf ("TWO-WAY SET ASSOCIATIVE\n");
  else if (org == FULLY)
    printf ("FULLY ASSOCIATIVE\n");
  else
    printf ("ERROR: WRONG CACHE ORG\n");
}

// show cache contents: DIRECT or FULLY
void showCacheAddress()
{
  for(int j=0; j<BLOCKS_IN_CACHE; j++) {
    printf("Address in block %d: ", j);
    for(int k=0; k<WORDS_PER_BLOCK; k++) {
      // print out addresses of each block
    }
    printf("\n");
  }
}

int getData (int address)                // load
{
  int data;
  // implement here
  return data;
}

void putData (int address, int value)     // store
{
  // implement here
}
