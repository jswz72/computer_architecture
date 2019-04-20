#ifndef MEMORY_HH_
#define MEMORY_HH_
#include <stdio.h>
#include <unordered_map>

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
  Block putData(int, int);
};//class MainMem
//-------------------------------------
//

struct LRUNode {
    LRUNode() {}
    LRUNode(Block block): block(block) {}
    Block block;
    LRUNode *next;
    LRUNode *prev;
    void remove();
};




//=================
//==  Cache
//=================
class Cache {

    class LRUQueue {
    public:
        LRUQueue() {
            capacity = BLOCKS_IN_CACHE;
            // Head points to before head
            head = new LRUNode();
            // Tail points to after tail (new nodes inserted at tail)
            tail = new LRUNode();
            head->next = tail;
            tail->prev = head;
        }

        void put(int address, int value);
        void put(int address);

    private:
        void add_node(LRUNode);
        void evict();
        int capacity; 
        LRUNode *head;
        LRUNode *tail;
        std::unordered_map<int, LRUNode*> node_map;
    };

public:
  Block cblocks[BLOCKS_IN_CACHE];
  MainMem MainMemory;
  Cache::LRUQueue lru_q;

  Cache(){ }  //constructor
  ~Cache(){ }  //destructor
  //=====================================
  int getData(int address);
  void putData( int address, int value );

private:
  int get_data_direct(int);
  void put_data_direct(int, int);
  int get_data_fully(int);
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
    for(int j=0; j<BLOCKS_IN_CACHE; j++) {
      printf("Address in block %d: ", j);
      for(int k=0; k<WORDS_PER_BLOCK; k++) {
	;// print out addresses of each block
      }
      printf("\n");
    }
  }
};

#endif
