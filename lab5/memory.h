#ifndef MEMORY_H_
#define MEMORY_H_
#include <stdio.h>

/*
  This file contains basic cache and block structures.
  It also defines some global constants
  for use in the classes and main program.
*/

extern unsigned int clockX;
extern unsigned int numMisses;
extern int cache_org;

//=================
//==  Constants
//=================
#define WORDS_PER_BLOCK 4
#define BLOCKS_IN_MEMORY 512
#define BLOCKS_IN_CACHE 8
#define NUM_OF_SET 4
#define BLOCKS_IN_SET 2 
//-------------------------------------


// block
typedef struct {
  int tag;
  int data[WORDS_PER_BLOCK];
  int last_used; // initialize to 0 ;
  int valid; // initialize to 0;
} Block; 

// main memory
typedef struct {
  Block blocks[BLOCKS_IN_MEMORY];
} MainMem;

MainMem mm;

enum cache_org {DIRECT = 0, FULLY, TWOWAY}; // FULLY=1, TWOWAY=2

// cache
typedef struct {
  Block cblocks[BLOCKS_IN_CACHE];
} Cache;

// memory
typedef struct {
  Cache myCache;
} Memory;

Memory m;

int getData (int address); // load
void putData (int address, int value); // store
void resetClock(void); // set timer to zero
void showCacheAddress(); // show cache contents
void printCacheOrg (int org); // print cache org

#endif
