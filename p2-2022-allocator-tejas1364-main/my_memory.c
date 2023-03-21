#include "my_memory.h"
#include <stdio.h>
#include <stdlib.h>

/*
Data Structures:
    Struct Chunk: Stores a chunk of free space in a list
    Struct Element: Used to store an array of different sizes (2^9 - 2^23)
    Struct Slab: Holds type, used, bitmap, and starting address. Used to create a slab linked list    

struct chunk{      <------ Located in header file
    long start;
    struct chunk *next;
}; */

typedef struct elementStruct{
    int size;
    int count;
    struct chunk *head;
}element;

struct slab{
    int type;
    int used;
    int bitMap[64];
    void* address;
    struct slab *next;
};

//GLOBAL VARIBALES
int count = 0;
struct slab *head = NULL;
static element array[15];


//************* Slab Functions *************


//Checks if a new Slab is needed, returns 1 if no, returns 0 if a new slab is needed
int checkExistence(int type){
    struct slab *temp;
    temp = head;
    
    for(int i = 0; i < count; i++){
        if (temp->type == type && temp->used < 64){
            return 1;
        }
				else if(temp->type == type && temp->used == 64){
	    		if(temp->next != NULL && temp->next->type == type && temp->next->used != 64){
						return 1;
					}
				}
    		temp = temp->next;
    }
    return 0;
}



//Sorts the slab list
void sortSlab(){
    struct slab *current, *next;
    int tempType, tempUsed, tempBitMap[64];
    void * tempAddress;
    //int count = array[index].count;
    int k = count;
    
    for(int i = 0; i < count - 1; i++, k--){
        current = head;
        
    	if(current != NULL){
            next = current->next;
        
            for(int j = 1; j < k; j++){
                if(current->type > next->type){
                    tempType = current->type;
                    current->type = next->type;
                    next->type = tempType;
                    
                    tempUsed = current->used;
                    current->used = next->used;
                    next->used = tempUsed;
                    
		            		tempAddress = current->address;
                    current->address = next->address;
                    next->address = tempAddress;
		    
                    for(int b = 0; b<64; b++){
                        tempBitMap[b] = current->bitMap[b];
                        current->bitMap[b] = next->bitMap[b];
                        next->bitMap[b] = tempBitMap[b];
                    }  
                }
		        		else if(current->type == next->type){
		            	if(current->address > next->address){
			            	tempUsed = current->used;
			            	current->used = next->used;
			            	next->used = tempUsed;
			                
			            	tempType = current->type;
                  	current->type = next->type;
                  	next->type = tempType;
			
			     
			            	tempAddress = current->address;
                  	current->address = next->address;
                  	next->address = tempAddress;
			     
			            	for(int b = 0; b<64; b++){
                  		tempBitMap[b] = current->bitMap[b];
                    	current->bitMap[b] = next->bitMap[b];
                    	next->bitMap[b] = tempBitMap[b];
                  	}
		            	}
		        		}
            		current = current->next;
            		next = next->next;
            }
        }
        current = current->next;
    }
}



//Adds a bit to the lowest address slab with same type
void *addBit(int type){
    struct slab *temp;
    long bitAddr;
    temp = head;
    for(int i = 0; i < count; i++){
        if ((temp->type == type) && (temp->used != 64)){
	        bitAddr = (long)temp->address;
	        for(int j = 0; j < 64; j++){
	            if(temp->bitMap[j] == 0){
		            temp->bitMap[j] = 1;
		            temp->used++;
		            return (void*)bitAddr;   
		        }
		        bitAddr = bitAddr + temp->type + 8;
	        }
        }
        temp = temp->next;
    }
    return NULL;
}



//Adds a slab
void addSlab(int type, void *address){
    struct slab *newSlab = (struct slab*)malloc(sizeof(struct slab));
    
    newSlab->address = address;	
    newSlab->type = type;
    newSlab->used = 1;
    newSlab->next = NULL;
	
    newSlab->bitMap[0] = 1;
    for(int i = 1; i < 64; i++){
        newSlab->bitMap[i] = 0;
    }  
    count++;
    if(head == NULL){
        head = newSlab;
    }
    else{
        struct slab *temp = head;
        
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = newSlab;
    }
    sortSlab();
}



//Deletes a slab at specified start address
void deleteSlab(int type, void *address){
    struct slab *temp, *prev;
    temp = head;

    if (temp != NULL && temp->type == type && (long)temp->address == (long)address){
        head = temp->next; 
        count--;
        free(temp);
	    return;
    }
    
    while (temp != NULL && temp->address != address && temp->type != type){
        prev = temp; 
        temp = temp->next;
    }
    
    if (temp == NULL){
        return;
    }
 
    prev->next = temp->next;
    free(temp);
    count--;
}



//Deletes a bit at specified slab
void *deleteBit(void *address){
    long addr = (long)address;
    long bitAddr;
    long tempStart;
    struct slab *temp;
    temp = head;
    
    if(temp->next == NULL){ //Only one slab
	    bitAddr = (long)temp->address; 
	    for(int j = 0; j < 64; j++){
            if(bitAddr == addr){
               temp->bitMap[j] = 0;
               temp->used--;

               if(temp->used == 0){
                  void *tempAddr = temp->address;
                  deleteSlab(temp->type, temp->address);
                  return tempAddr;
               }
               break;
            }
            bitAddr = bitAddr + temp->type + 8;
	    }
	    return NULL;
    }
    
    for(int i = 0; i < count; i++){ //More than one slab
        if(address >= temp->address){
					if(temp->next != NULL && address < temp->next->address) {
						bitAddr = (long)temp->address; 
						for(int j = 0; j < 64; j++){
							if(bitAddr == addr){
								temp->bitMap[j] = 0;
								temp->used--;

								if(temp->used == 0){
						  		void *tempAddr = temp->address;
									deleteSlab(temp->type, temp->address);
									return tempAddr;
								}
								break;
				    	}
				    	bitAddr = bitAddr + temp->type + 8;
	    	  	}
		    	}
					else{
	        	bitAddr = (long)temp->address; 
	    			for(int j = 0; j < 64; j++){
							if(bitAddr == addr){
								temp->bitMap[j] = 0;
								temp->used--;
								if(temp->used == 0){
									void *tempAddr = temp->address;
						   		deleteSlab(temp->type, temp->address);
									return tempAddr;
								}
								break;
							}
							bitAddr = bitAddr + temp->type + 8;
	    			}
					}
	   			return NULL;
	    	}
        temp = temp->next;
    }
    return NULL;
}
	

//************* Buddy Functions *************


//Initialize element array that holds free chunks
void init_array(int size){
    for(int i=0; i < 15; i++){
        array[i].head = NULL;
        array[i].size = size;
        size = size/2;
    }
}



//Sort Chunk Function
void sortChunk(int index){
    struct chunk *current;
    struct chunk *next;
    long temp_start;
    int count = array[index].count;
    int k = array[index].count;
    
    for(int i = 0; i < count - 1; i++, k--){
        current = array[index].head;
	    if(current != NULL){
            next = array[index].head->next;
        
            for(int j = 1; j < k; j++){
                if(current->start > next->start){
                    temp_start = current->start;
                
                    current->start = next->start;
                    next->start = temp_start;
                }      
                current = current->next;
                next = next->next;
            }
	    }
    }
}


//HELPER FUNCTIONS 

//Get Address Function
long getStart(struct chunk *temp){
    return temp->start;
}
//Get Next Address Function
long getNextStart(struct chunk *temp){
    return temp->next->start;
}
//Get Address Function
long getAddress(int index){
	if(array[index].head != NULL){
  		return array[index].head->start;
	}
	else{
		return 0;
	}
}
//Returns number of chunks at a specific index 
int getCount(int index){
    return array[index].count;
}

//END OF HELPER FUNCTIONS


//Add Chunk Function
void addChunk(int index, long address){
    struct chunk *newChunk = (struct chunk*)malloc(sizeof(struct chunk));
    struct chunk *temp;
    newChunk->next = NULL;
    newChunk->start = address;

    if(array[index].head == NULL){
        array[index].head = newChunk;
    }
    else{
	    temp = array[index].head;
	    while(temp->next != NULL){
		    temp = temp->next;
	    }
	    temp->next = newChunk;
    }
    array[index].count++;
    sortChunk(index);
}



//Delete Chunk Function
void deleteChunk(int index){
    struct chunk *temp;
    if(array[index].head != NULL){
        temp = array[index].head;
        array[index].head = array[index].head->next;
        free(temp);
    }
    array[index].count--;
}



//Delete Specific Chunk Function
void deleteSpecificChunk(int index, long addr){
    struct chunk *temp1 = array[index].head, *prev;
    
    if (temp1 != NULL && temp1->start == addr) {
        array[index].head = temp1->next; 
        free(temp1);
	    array[index].count--;
	    return;
    }
    while (temp1 != NULL && temp1->start != addr) {
        prev = temp1; 
        temp1 = temp1->next;
    }
    if (temp1 != NULL){
        prev->next = temp1->next;
    	free(temp1);
	    array[index].count--;
    }
}



//Split Function
void splitChunk(int size, int index) {
    int i = index;
    if(array[i].size != size && array[i].head != NULL) {
        long startAddress = array[i].head->start;
	    long s = array[i+1].size;
        deleteChunk(i);
        addChunk(i+1, startAddress);
        addChunk(i+1, startAddress + s);
        splitChunk(size, i + 1);
    }
}



//Find Index Function
int findIndex(int size){
    for(int i = 0; i < 15; i++){ //For loop that iterates through each of the powers of 2 from 2^9-2^23
        //If the size of the array[i] is the same as req size and the free list at that size is not empty, return i
        if(array[i].size == size && array[i].head != NULL){
            return i;
        }
        //Else if the free holes list is empty
        else if(array[i].size == size && array[i].head == NULL){
            for(int j = i - 1; j >= 0; j--){
                //Traverse backwards in the array until the first hole is found, split at that index
                if(array[j].head != NULL){
                    splitChunk(size, j);
                    return i;
                }
            }  
        }
    }
    return -1; //Return -1 if no index is found (Indicating no memory available)
}



//Find Free Index
int findFreeIndex(int size){
    for(int i = 0; i < 15; i++){ //For loop that iterates through each of the powers of 2 from 2^9-2^23
        //If the size of the array[i] is the same as req size and the free list at that size is not empty, return i
        if(array[i].size == size){
            return i;
        }
    }
}



//Recursive Combine chunks function
//if buddy, then combine
void combineChunks(int index) {
    struct chunk *c;
    long chunk1, chunk2;
    int count, size;

    count = array[index].count;
    if(count > 1 && index != 0){
    	size = array[index].size;
	    c = array[index].head;
    	for(int i = 0; i < count; i++){
	        chunk1 = (c->start/size);
	        if(chunk1 % 2 == 0 && c->next != NULL){
		        chunk2 = (c->next->start/size);
		        if(chunk1 + 1 == chunk2){
		            addChunk(index-1, getStart(c));
		            deleteSpecificChunk(index, getStart(c));
		            deleteSpecificChunk(index, getNextStart(c));
		            combineChunks(index-1);
		            break;
		        }
	        }
	        if(c->next != NULL){
		        c = c->next;
	        }
	    }
    }
}



//Print Function Used For Testing
void print(){
    struct chunk *temp;
    for(int i = 0; i < 15; i++){
			printf("\n%d, ", array[i].size);
			temp = array[i].head;
				if(temp != NULL){
					for(int j = 0; j < array[i].count; j++){
						printf("%ld, ", temp->start);
						temp = temp->next;
					}
				}
		}
}




