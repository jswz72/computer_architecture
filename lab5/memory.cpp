#include <iostream>
#include "memory.hh"

using std::cout;
using std::endl;


unsigned int clockX;
unsigned int numMisses;
unsigned int counter;

int cache_org;

void resetClock()
{
  clockX = 0;
  numMisses = 0;
  counter = 0;
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

void mem_access() {
    clockX += 100;
}

void cache_access() {
    clockX += 2;
}

void Cache::put_data_fully(int address, int value) {
    cache_access();
    int tag = address >> 2;
    //cout << "Put Address: " << address << " Tag: " << tag << endl;
    Block block;
    bool found = false;
    for (int i = 0; i < BLOCKS_IN_CACHE; i++) {
        if (cblocks[i].tag == tag) {
            found = true;
            block = cblocks[i];
            break;
        }
    }
    // Write-through
    if (found) {
        MainMemory.putData(address, value);
    }
    // Not in queue/cache
    else {
        // Update and get it from main_memory
        numMisses++;
        MainMemory.putData(address, value);
        block = MainMemory.getData(address);
    }
    // Add new node as most current
    block.last_used = counter;
    add_block(block);
}

// Add block to empty space.
// If no empty space, evict least recently used
void Cache::add_block(Block &block) {
    int least = 0;
    int index = 0;
    for (int i = 0; i < BLOCKS_IN_CACHE; i++) {
        // End early if open block
        if (!cblocks[i].valid) {
            index = i;
            break;
        }
        int diff = counter - cblocks[i].last_used > least;
        if (diff > least) {
            least = diff;
            index = i;
        }
    }
    cblocks[index] = block;
}

int Cache::get_data_fully(int address) {
    cache_access();
    // TODO
    int block_offset = address & 0x3;
    int tag = address >> 2;
    //cout << "Get Address: " << address << " Tag: " << tag << endl;
    Block block;
    bool found = false;
    for (int i = 0; i < BLOCKS_IN_CACHE; i++) {
        if (cblocks[i].tag == tag) {
            found = true;
            block = cblocks[i];
            break;
        }
    }
    if (!found) {
        // Not found in cache
        numMisses++;
        block = MainMemory.getData(address);
    }
    block.last_used = counter;
    add_block(block);
    return block.data[block_offset];
}

Block MainMem::getData(int address) {
    mem_access();
    int addr_data = address >> 2; // dont care about blockoffset
    int mem_idx = addr_data & 0x1FF; // 511
    //cout << "Get addr " << address << " idx: " << mem_idx << endl;
    return blocks[mem_idx];
}

void MainMem::putData(int address, int value) {
    mem_access();
    int addr_data = address;
    int block_offset = addr_data & 0x3;
    addr_data >>= 2;
    int mem_idx = addr_data & 0x1FF; // 511
    int tag = addr_data >> 3;
    blocks[mem_idx].valid = true;
    blocks[mem_idx].tag = tag;
    //cout << "Put addr " << address << " idx: " << mem_idx << endl;
    blocks[mem_idx].data[block_offset] = value;
}

// Get data using direct mapped cache
int Cache::get_data_direct(int address) {
    cache_access();
    int addr_data = address;
    int block_offset = addr_data & 0x3;
    addr_data >>= 2;
    int cache_idx = addr_data & 0x7;
    addr_data >>= 3;
    int tag = addr_data;
    cout << "Get addr " << address << " idx " << cache_idx << endl;

    if (!cblocks[cache_idx].valid || cblocks[cache_idx].tag != tag) {
        numMisses++;
        Block block = MainMemory.getData(address);
        // TODO doesn't work when take this out
        //block.valid = true;
        //block.tag = tag;
        cblocks[cache_idx] = block;
        return block.data[block_offset];
    } else {
        return cblocks[cache_idx].data[block_offset];
    }
}


// Put data using direct mapped cache
void Cache::put_data_direct(int address, int value) {
    cache_access();
    int addr_data = address >> 2; // don't care about block offset
    int cache_idx = addr_data & 0x7;
    addr_data >>= 3;
    int tag = addr_data;
    cout << "Put addr " << address << " idx " << cache_idx << endl;

    if (!cblocks[cache_idx].valid || cblocks[cache_idx].tag != tag) {
        numMisses++;
        MainMemory.putData(address, value);
        Block block = MainMemory.getData(address);
        cblocks[cache_idx] = block;
    } else {
        MainMemory.putData(address, value);
    }
}

int Cache::getData(int address)
{
    int data;
    if (cache_org == DIRECT)
        data = get_data_direct(address);
    else if (FULLY)
        data = get_data_fully(address);
    else if (TWOWAY)
        data = 0;
    counter++;
    return data;
}

void Cache::putData(int address, int value)
{
    if (cache_org == DIRECT)
        put_data_direct(address, value);
    else if (FULLY) {
        put_data_fully(address, value);
    }
    counter++;
}
