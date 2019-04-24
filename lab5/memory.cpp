#include <iostream>
#include <unordered_map>
#include <vector>
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

void mem_access() {
    clockX += 100;
}

void cache_access() {
    clockX += 2;
}


std::vector<int> LRUQueue::contents() {
    std::vector<int> contents;
    LRUNode *runner = tail->prev;
    while (runner != nullptr) {
        contents.push_back(runner->block.tag);
        runner = runner->prev;
    }
    return contents;
}

void LRUQueue::print_contents() {
    std::vector<int> c= contents();
    cout << "Contents:";
    for (int i = 0; i < c.size(); i++) {
        cout << " " << c[i];
    }
    cout << endl;
}

void LRUQueue::put(int address, int value) {
    cache_access();
    int tag = address >> 2;
    cout << "Put Address: " << address << " Tag: " << tag << endl;
    Block block;
    if (node_map.find(tag) != node_map.end()) {
        // Remove current node from point in queue if is in cache
        block = node_map[tag]->block;
        // Write through, don't care about return
        main_mem->putData(address, value);
        node_map[tag]->remove();
    }
    // Not in queue/cache
    else {
        // Update and get it from main_memory
        main_mem->putData(address, value);
        numMisses++;
        block = main_mem->getData(address);
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
    cache_access();
    // TODO
    int block_offset = address & 0x3;
    int tag = address >> 2;
    cout << "Get Address: " << address << " Tag: " << tag << endl;
    Block block;
    if (node_map.find(tag) != node_map.end()) {
        block = node_map[tag]->block;
        node_map[tag]->remove();
    } else {
        // Not found in cache
        numMisses++;
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
    prev->next = next;
    next->prev = prev;
}

// TODO write tag?
Block MainMem::getData(int address) {
    mem_access();
    int addr_data = address >>= 2; // dont care about blockoffset
    int mem_idx = addr_data & 0x1FF; // 511
    return blocks[mem_idx];
}

// TODO write tag?
void MainMem::putData(int address, int value) {
    mem_access();
    int addr_data = address;
    int block_offset = addr_data >>= 2;
    int mem_idx = addr_data & 0x1FF; // 511
    int tag = addr_data >>= 3;
    blocks[mem_idx].tag = tag;
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

    if (!cblocks[cache_idx].valid || cblocks[cache_idx].tag != tag) {
        numMisses++;
        Block block = MainMemory.getData(address);
        block.tag = tag;
        block.valid = 1;
        cblocks[cache_idx] = block;
        return block.data[block_offset];
    } else {
        return cblocks[cache_idx].data[block_offset];
    }
}


// Put data using direct mapped cache
void Cache::put_data_direct(int address, int value) {
    cache_access();
    int addr_data = address >>= 2; // don't care about block offset
    int cache_idx = addr_data & 0x7;
    addr_data >>= 3;
    int tag = addr_data;

    if (!cblocks[cache_idx].valid || cblocks[cache_idx].tag != tag) {
        numMisses++;
        MainMemory.putData(address, value);
        Block block = MainMemory.getData(address);
        block.tag = tag;
        block.valid = 1;
        cblocks[cache_idx] = block;
    } else {
        MainMemory.putData(address, value);
    }
}

int Cache::getData(int address)
{
    int data = 0; // TODO temp to surpress warnings
    if (cache_org == DIRECT)
        data = get_data_direct(address);
    else if (FULLY)
        data = lru_q.get(address);
    return data;
}

void Cache::putData(int address, int value)
{
    if (cache_org == DIRECT)
        put_data_direct(address, value);
    else if (FULLY) {
        lru_q.put(address, value);
        //lru_q.print_contents();
    }
}
