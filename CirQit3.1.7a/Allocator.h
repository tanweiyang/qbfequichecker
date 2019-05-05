#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <assert.h>
#include <vector>
#include <new>
#include <iostream>
#include <stack>

#include "Common.h"
using namespace std;


#define BLOCK_SIZE 1000 
// each node is ~ 100, so this is about 10 nodes


// Allocates objects of a given size, and when done, deallocates all at once

class Allocator
{
    vector<void*> block_list;
    void * mem_block;
    unsigned amt_free;
    
    void createNewBlock();
    
  public:
    Allocator();
    ~Allocator();
    void * alloc( size_t size );

};

void* operator new[]( size_t size, Allocator * allocator );
void* operator new( size_t size, Allocator * allocator );



class stackAlloc
{
private:
	stack<void*> block_list;
	void * mem_block;

	int block_size;

public:
	stackAlloc(int b_size = BLOCK_SIZE);

	// Push a new chunk on the stack
	//   of the given size 
	//   and return a pointer to the memory
	void *getNew(size_t size);
	
	// Return the pointer to the last object
	void *seeLast();

	void forgetLast();
};


inline void *mallocC(size_t size)
{
        void *v = malloc(size);
        assert(v);
        return v;
}


#endif
