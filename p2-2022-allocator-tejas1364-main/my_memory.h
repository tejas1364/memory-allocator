#ifndef MY_MEMORY_H
#define MY_MEMORY_H

#include "interface.h"

// Chunk Definition
struct chunk{
    long start;
    struct chunk *next;
}; 


// Slab Functions
void *addBit(int type);
void addSlab(int type, void *address);

void *deleteBit(void *address);
void deleteSlab(int type, void *address);

void sortSlab();
int checkExistence(int type);


// Buddy Functions
long getStart(struct chunk *temp);
long getNextStart(struct chunk *temp);
long getAddress(int index);
int getCount(int index);
void print();

void init_array(int size);
void addChunk(int index, long address);
void deleteChunk(int index);
void deleteSpecificChunk(int index, long addr);

int findIndex(int size);
int findFreeIndex(int size);
void splitChunk(int size, int index);
void sortChunk(int index);
void combineChunks(int index);


#endif
