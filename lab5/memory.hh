#ifndef MEMORY_HH_
#define MEMORY_HH_
#include <stdio.h>
#include <iostream>

using std::cout;
using std::endl;

/*
  This file contains basic cache and block structures.
  It also defines some global constants
  for use in the classes and main program.
*/

extern unsigned int clockX;
extern unsigned int numMisses;
extern int cache_org;

enum cache_org {DIRECT = 0, FULLY, TWOWAY}; // FULLY=1, TWOWAY=2

void printCacheOrg (int org); // print cache org
void resetClock(void); // set timer to zero

//=================
//==  Constants
//=================
#define WORDS_PER_BLOCK 4
#define BLOCKS_IN_MEMORY 512
#define BLOCKS_IN_CACHE 8
#define NUM_OF_SET 4
//-------------------------------------

//=================
//==  Block
//=================
class Block{
 public:
  Block()//constructor
  {
    valid = 0;
    // Need to initialize for later comparison
    last_used = 0;
  }
  ~Block(){ }//destructor
  
  int tag;
  int data[WORDS_PER_BLOCK];
  int last_used;
  int valid;
};//class block
//-------------------------------------


//=================
//==  MainMem
//=================
class MainMem{
 public:
  Block blocks[BLOCKS_IN_MEMORY];
  MainMem(){ }  //constructor
  ~MainMem(){ }  //destructor
  Block getData(int);
  void putData(int, int);
};//class MainMem
//-------------------------------------
//


//=================
//==  Cache
//=================
class Cache {
public:
  Block cblocks[BLOCKS_IN_CACHE];
  MainMem MainMemory;

  Cache(){}
  ~Cache(){ }
  //=====================================
  int getData(int address);
  void putData( int address, int value );

private:
  int get_data_direct(int);
  void put_data_direct(int, int);
  int get_data_fully(int);
  void put_data_fully(int, int);
  int get_data_twoway(int);
  void put_data_twoway(int, int);
  void add_block_fully(Block block);
  void add_block_twoway(Block block, int set);
  void print_cache();
};//class Cache


//-------------------------------------


class Memory
{
private:
  Cache myCache;
  void show_cache_direct();
  void show_cache_fully();
  void show_cache_twoway();
  
public:
  
  Memory (){}                                   // constructor
  
  int  getData ( int address )                // load
  {
    return myCache.getData(address);
  }
  void putData ( int address, int value )     // store
  {
    myCache.putData(address, value);
  }

  void showCacheAddress () // show the cache contents
  {
      if (cache_org == DIRECT)
          show_cache_direct();
      else if (cache_org == FULLY)
          show_cache_fully();
      else 
          show_cache_twoway();
  }
};

#endif
