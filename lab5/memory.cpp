#include <iostream>
#include <unordered_map>
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

void LRUQueue::put(int address, int value) {
    // TODO
    int tag = address >>= 5;
    Block block;
    if (node_map.find(tag) != node_map.end()) {
        // Remove current node from point in queue if is in cache
        hit();
        block = node_map[tag]->block;
        // Write through, don't care about return
        main_mem->putData(address, value);
        node_map[tag]->remove();
    }
    // Not in queue/cache
    else {
        miss();
        // Update and get it from main_memory
        block = main_mem->putData(address, value);
        // Queue full - make space
        if (node_map.size() == capacity)
            evict();
    }
    // Add new node as most current
    LRUNode *cur = new LRUNode(block);
    add_node(cur);
    node_map[tag] = cur;
}

// Get rid of LRU node from queue, remove entry from map
void LRUQueue::evict() {
    LRUNode *to_remove = head->next;
    to_remove->remove();
    node_map.erase(node_map.find(to_remove->block.tag));
}

int LRUQueue::get(int address) {
    // TODO
    int block_offset = address & 0x3;
    int tag = address >>= 5;
    Block block;
    if (node_map.find(tag) != node_map.end()) {
        hit();
        block = node_map[tag]->block;
        node_map[tag]->remove();
    } else {
        // Not found in cache
        miss();
        block = main_mem->getData(address);
        // Queue full
        if (node_map.size() == capacity)
            evict();
    }
    LRUNode *cur = new LRUNode(block);
    add_node(cur);
    node_map[tag] = cur;
    return block.data[block_offset];
}

void LRUQueue::add_node(LRUNode *node) {
    LRUNode *last = tail->prev;
    last->next = node;
    node->prev = last;
    node->next = tail;
    tail->prev = node;
}

// Remove self from LL queue
void LRUNode::remove() {
    LRUNode *prev = prev;
    LRUNode *next = next;
    prev->next = next;
    next->prev = prev;
    next = 0;
    prev = 0;
}

// TODO write tag?
Block MainMem::getData(int address) {
    int addr_data = address >>= 2; // blockoffset
    int mem_idx = addr_data & 0x1FF; // 511
    return blocks[mem_idx];
}

// TODO write tag?
Block MainMem::putData(int address, int value) {
    int addr_data = address; // blockoffset
    int block_offset = addr_data >>= 2;
    int mem_idx = addr_data & 0x1FF; // 511
    blocks[mem_idx].data[block_offset] = value;
    return blocks[mem_idx];
}

// Get data using direct mapped cache
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

// Put data using direct mapped cache
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

// Get data using fully associative cache
int Cache::get_data_fully(int address) {
    lru_q.get(address);
	return 0;	// TODO tmp to supress warnings
}

int Cache::getData(int address)
{
    int data = 0; // TODO temp to surpress warnings
    if (cache_org == DIRECT)
        data = get_data_direct(address);
    else if (FULLY)
        data = get_data_fully(address);
    return data;
}

void Cache::putData(int address, int value)
{
    if (cache_org == DIRECT)
        put_data_direct(address, value);
}
