#include <iostream>
#include "memory.hh"

using std::cout;
using std::endl;

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

void miss() {
    clockX += 100;
    numMisses++;
}

void hit() {
    clockX += 2;
}

Block MainMem::getData(int address) {
    int addr_data = address >>= 2; // blockoffset
    int mem_idx = addr_data & 0x1FF; // 511
    return blocks[mem_idx];
}

Block MainMem::putData(int address, int value) {
    int addr_data = address; // blockoffset
    int block_offset = addr_data >>= 2;
    int mem_idx = addr_data & 0x1FF; // 511
    blocks[mem_idx].data[block_offset] = value;
    return blocks[mem_idx];
}

int Cache::get_data_direct(int address) {
    int addr_data = address;
    int block_offset = addr_data & 0x3;
    addr_data >>= 2;
    int cache_idx = addr_data & 0x7;
    addr_data >>= 3;
    int tag = addr_data;

    if (!cblocks[cache_idx].valid || cblocks[cache_idx].tag != tag) {
        miss();
        Block block = MainMemory.getData(address);
        block.tag = tag;
        block.valid = 1;
        cblocks[cache_idx] = block;
        return block.data[block_offset];
    } else {
        hit();
        return cblocks[cache_idx].data[block_offset];
    }
}

void Cache::put_data_direct(int address, int value) {
    int addr_data = address >>= 2; // don't care about block offset
    int cache_idx = addr_data & 0x7;
    addr_data >>= 3;
    int tag = addr_data;

    if (!cblocks[cache_idx].valid || cblocks[cache_idx].tag != tag) {
        miss();
        Block block = MainMemory.putData(address, value);
        block.tag = tag;
        block.valid = 1;
        cblocks[cache_idx] = block;
    } else {
        hit();
        Block block = MainMemory.putData(address, value);
    }
}

int Cache::getData(int address)
{
    int data = 0; // TODO temp to surpress warnings
    if (cache_org == DIRECT)
        data = get_data_direct(address);
    return data;
}

void Cache::putData(int address, int value)
{
    if (cache_org == DIRECT)
        put_data_direct(address, value);
}
