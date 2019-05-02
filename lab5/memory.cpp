#include <iostream>
#include <cassert>
#include "memory.hh"

using std::cout;
using std::endl;


unsigned int clockX;
unsigned int numMisses;
unsigned int counter = 0;

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

void mem_access() {
    clockX += 100;
}

void cache_access() {
    clockX += 2;
}

void Memory::show_cache_direct() { 
    for(int i = 0; i < BLOCKS_IN_CACHE; i++) { 
        Block block = myCache.cblocks[i]; 
        cout << "Address in block " << i;
        cout << " (set " << i << ", way 0):"; 
        for(int j = 0; j < WORDS_PER_BLOCK; j++) { 
            int addr = (((block.tag << 3) + i) << 2 ) + j;
            cout << " " << addr;
        }
        cout << endl;
    }
}

void Memory::show_cache_fully() {
    for (int i = 0; i < BLOCKS_IN_CACHE; i++) {
        Block block = myCache.cblocks[i];
        cout << "Address in block " << i;
        cout << " (set 0, way " << i << "):"; 

        for (int j = 0; j < WORDS_PER_BLOCK; j++) {
            int addr = (block.tag << 2) + j;
            cout << " " << addr;
        }
        cout << " last used: " << counter - block.last_used << endl;
    }
}

void Memory::show_cache_twoway() {
    for (int i = 0; i < BLOCKS_IN_CACHE; i++) {
        Block block = myCache.cblocks[i];
        cout << "Address in block " << i;
        cout << " (set " << i % NUM_OF_SET;
        cout << ", way " << i / NUM_OF_SET << "):"; 

        for (int j = 0; j < WORDS_PER_BLOCK; j++) {
            int addr = (block.tag << 2) + j;
            cout << " " << addr;
        }
        cout << " last used: " << counter - block.last_used << endl;
    }
}

// Return block containing given address
Block MainMem::getData(int address) {
    mem_access();
    int addr_data = address >> 2;
    int mem_idx = addr_data & 0x1FF; // 511

    return blocks[mem_idx];
}

// Write value to address in corresponding memory block
void MainMem::putData(int address, int value) {
    mem_access();
    int addr_data = address;
    int block_offset = addr_data & 0x3;
    addr_data >>= 2;
    int mem_idx = addr_data & 0x1FF; // 511
    blocks[mem_idx].valid = true;
    blocks[mem_idx].data[block_offset] = value;
}

// Add block to empty space in given set
void Cache::add_block_twoway(Block block, int set) {
    int least = 0, index = 0;
    for (int i = set; i < BLOCKS_IN_CACHE; i += NUM_OF_SET) {
        if (!cblocks[i].valid) {
            index = i;
            break;
        }
        int diff = counter - cblocks[i].last_used;
        if (diff > least) {
            least = diff;
            index = i;
        }
    }
    cblocks[index] = block;
}

// Add block to empty space in fully associative cache
// If no empty space, evict least recently used block in cache
void Cache::add_block_fully(Block block) {
    int least = 0;  // least recent time
    int index = 0;  // index of LRU block
    for (int i = 0; i < BLOCKS_IN_CACHE; i++) {
        // End early if open block
        if (!cblocks[i].valid) {
            index = i;
            break;
        }
        int diff = counter - cblocks[i].last_used;
        if (diff > least) {
            least = diff;
            index = i;
        }
    }
    cblocks[index] = block;
}

// Get data at address using two way associative cache
int Cache::get_data_twoway(int address) {
    cache_access();
    int block_offset = address & 0x03;
    int tag = address >> 2;
    int my_set = tag % NUM_OF_SET;
    Block block;
    bool found = false;
    // Search set for block
    for (int i = my_set; i < BLOCKS_IN_CACHE; i += NUM_OF_SET) {
        if (cblocks[i].tag == tag) {
            found = true;
            // Update for LRU
            cblocks[i].last_used = counter;
            block = cblocks[i];
            break;
        }
    }
    // Add block to cache and update block data if not found
    if (!found) {
        numMisses++;
        block = MainMemory.getData(address);
        block.last_used = counter;
        block.tag = tag;
        block.valid = true;
        add_block_twoway(block, my_set);
    }
    return block.data[block_offset];
}

// Put data value at address using two way associative cache
void Cache::put_data_twoway(int address, int value) {
    cache_access();
    int block_offset = address & 0x03;
    int tag = address >> 2;
    int my_set = tag % NUM_OF_SET;
    bool found = false;
    // Search set for block
    for (int i = my_set; i < BLOCKS_IN_CACHE; i += NUM_OF_SET) {
        if (cblocks[i].tag == tag) {
            found = true; 
            cblocks[i].data[block_offset] = value;
            cblocks[i].last_used = counter;
            break;
        }
    }
    // Write-through
    if (found) {
        MainMemory.putData(address, value);
    }
    // Get block, add to cache and update block data if not found
    else {
        numMisses++;
        MainMemory.putData(address, value);
        Block block = MainMemory.getData(address);
        block.last_used = counter;
        block.tag = tag;
        block.valid = true;
        add_block_twoway(block, my_set);
    }
}

// Get data at address using fully associative cache
int Cache::get_data_fully(int address) {
    cache_access();
    int block_offset = address & 0x3;
    int tag = address >> 2;
    Block block;
    bool found = false;
    // Search cache for block
    for (int i = 0; i < BLOCKS_IN_CACHE; i++) {
        if (cblocks[i].tag == tag) {
            found = true;
            cblocks[i].last_used = counter;
            block = cblocks[i];
            break;
        }
    }
    if (!found) {
        // Not found in cache
        numMisses++;
        block = MainMemory.getData(address);
        block.last_used = counter;
        block.tag = tag;
        block.valid = true;
        add_block_fully(block);
    }
    return block.data[block_offset];
}

// Put data value at address using fully associative cache
void Cache::put_data_fully(int address, int value) {
    cache_access(); 
    int block_offset = address & 0x03;
    int tag = address >> 2;
    bool found = false;
    for (int i = 0; i < BLOCKS_IN_CACHE; i++) {
        if (cblocks[i].tag == tag) {
            found = true; 
            cblocks[i].data[block_offset] = value;
            cblocks[i].last_used = counter;
            break;
        }
    }
    // Write-through
    if (found) {
        MainMemory.putData(address, value);
    }
    // Not in cache
    else {
        // Update and get it from main_memory
        numMisses++;
        MainMemory.putData(address, value);
        Block block = MainMemory.getData(address);
        block.last_used = counter;
        block.tag = tag;
        block.valid = true;
        add_block_fully(block);
    }
}

// Get data at address using direct mapped cache
int Cache::get_data_direct(int address) {
    cache_access();
    int addr_data = address;
    int block_offset = addr_data & 0x3;
    addr_data >>= 2;
    int cache_idx = addr_data & 0x7;
    addr_data >>= 3;
    int tag = addr_data;

    // Block isn't valid or not found
    if (!cblocks[cache_idx].valid || cblocks[cache_idx].tag != tag) {
        numMisses++;
        Block block = MainMemory.getData(address);
        block.tag = tag;
        block.valid = true;
        cblocks[cache_idx] = block;
        return block.data[block_offset];
    } else {
        return cblocks[cache_idx].data[block_offset];
    }
}


// Put data value at address using direct mapped cache
void Cache::put_data_direct(int address, int value) {
    cache_access(); 
    int block_offset = address & 0x3;
    int addr_data = address >> 2;
    int cache_idx = addr_data & 0x7;
    addr_data >>= 3;
    int tag = addr_data;

    // Block isn't valid or not found
    if (!cblocks[cache_idx].valid || cblocks[cache_idx].tag != tag) {
        numMisses++;
        // Write then read back
        MainMemory.putData(address, value);
        Block block = MainMemory.getData(address);
        block.tag = tag;
        block.valid = true;
        cblocks[cache_idx] = block;
    } else { 
        cblocks[cache_idx].data[block_offset] = value; 
        MainMemory.putData(address, value);
    }
}

// Get data at address using currently chosen cache organization
int Cache::getData(int address)
{
    counter++;  // Increment counter to determine LRU
    int data;
    if (cache_org == DIRECT)
        data = get_data_direct(address);
    else if (cache_org == FULLY) {
        data = get_data_fully(address);
    }
    else if (cache_org == TWOWAY)
        data = get_data_twoway(address);
    else { 
        cout << "Bad cache org" << endl;
        return 0; 
    }
    return data;
}

// Put data value at address using currently chosen cache organization
void Cache::putData(int address, int value)
{
    counter++;  // Increment counter to determine LRU
    if (cache_org == DIRECT)
        put_data_direct(address, value);
    else if (cache_org == FULLY)
        put_data_fully(address, value);
    else if (cache_org == TWOWAY) 
        put_data_twoway(address, value);
    else
        cout << "Bad cache org" << endl;
}
