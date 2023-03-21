#include "interface.h"
#include "my_memory.h"
#include <math.h>

//GLOBAL VARIABLES
void *mem_start;
int malloc_type;
int memory_size;
long mem;

/*
 * Function: my_setup
 * --------------------
 * Parameters: Malloc Type, Memory Size, Start address of Memory
 * 
 * Stores parameters in Global Variables and adds a chunk to Free List
*/
void my_setup(enum malloc_type type, int mem_size, void *start_of_memory)
{
  mem_start = start_of_memory;
  malloc_type = type;
  memory_size = mem_size;
  mem = (long) mem_start;
  init_array(MEMORY_SIZE);
  addChunk(0,0);
}


/*
 * Function: my_malloc
 * --------------------
 * Depeding on malloc type, we malloc a certain amount of memory by removing a chunk in our free list
 *    
 *  size: amount of memory we want to allocate
 *
 *  returns: starting address of memory we allocated
*/
void *my_malloc(int size){

  if(malloc_type == 0){ //Buddy Malloc
    int s = size + 8;
    if(size < MIN_MEM_CHUNK_SIZE){
      s = MIN_MEM_CHUNK_SIZE;
    }
    else{
      s = pow(2, ceil(log(s)/log(2))); 
    }
    
    int index = findIndex(s); //function that returns the index we are allocating space in

    if(index > -1){
      long addr = getAddress(index); //Must figure out type cast here and when doing the address in split
      addr = addr + mem; //Add original mem size back
      deleteChunk(index);
      *(int *) addr = size; //Allocate size in the previous 8 bytes of addr 
      return (void *) addr + HEADER_SIZE;
    }
    else{
      return NULL;
    }
  }
  else{ //Slab Malloc
    int s = (size+8)*64;
    
    if(checkExistence(size) == 0){ //None exist
      malloc_type = 0;
      void *start = my_malloc(s); //Allocate a slab with s size of memory
      malloc_type = 1;
      
      if(start != NULL){
        addSlab(size, start + 8);
        return (void *) ((long) start + 8);
      }
      else{
        return NULL;
      }
    }
    else{
       void *addr = addBit(size);
       return (void *)addr;
    }
  }
 }


/*
 * Function: my_free
 * --------------------
 * Depeding on malloc type, we free a certain amount of memory by adding a chunk in our free list
 *    
 *  ptr: starting address of memory we want to free
 *
*/
void my_free(void *ptr)
{
   if(malloc_type == 0){ //Buddy Free
    long addr = (long) ptr; //convert pointer into long type
    long header_size = 8;
    ptr = ptr - 8; //Header
    int size = ((int*)ptr)[0];
    size = size + 8;
     
    if(size < MIN_MEM_CHUNK_SIZE){
      size = MIN_MEM_CHUNK_SIZE;
    }
    else{
      size = pow(2, ceil(log(size)/log(2))); 
    }

    int index = findFreeIndex(size); //Calculate corrent index to add chunk

    addChunk(index, addr-header_size-mem); //Add Chunk into Free List

    if(getCount(index) > 1) { //Combine Chunk Check
      combineChunks(index);
    }
    if(getCount(0) == 1){ //Resets Memory (May not be needed)
      deleteChunk(0);
      init_array(MEMORY_SIZE);
      addChunk(0,0);
    }
  }
  else{ //Slab Free
    void *callMyFree = deleteBit(ptr); //Deletes a bit from proper bitmap, returns an adderss if slab needs to be freed
    if(callMyFree != NULL){
      malloc_type = 0;
      my_free(callMyFree - 8);
      malloc_type = 1;
    }
  }
}
