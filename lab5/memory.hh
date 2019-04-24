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
extern unsigned int counter;
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
  void add_block(Block &block);
};//class Cache


//-------------------------------------


class Memory
{
private:
  Cache myCache;
  
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
      for(int j = 0; j < BLOCKS_IN_CACHE; j++) { 
          cout << "Address in block " << j; 
          for(int k = 0; k < WORDS_PER_BLOCK; k++) { 
              Block block = myCache.cblocks[j];
              int addr = (((block.tag << 3) + j) << 2 ) + k;
              cout << " " << addr << " " << block.last_used;
          }
          cout << endl;
      }
  }
};

#endif
