#ifndef MEMCHECKER_H
#define MEMCHECKER_H

/*
 * This is a simple dynamic memory leak detector. I simply overload global 
 * operator new and delete and do a simple counter, that for every "new" there
 * must be a "delete"
 */


//#ifdef DEBUG_MEM_LEAKS 

#include <new>
#include <cstdlib>


namespace memchecker_NS
{

extern int mem_counter_;
extern int mem_array_counter_;

}


void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *ptr);
void operator delete[](void *ptr);


//#endif // DEBUG_MEM_LEAKS 


#endif // MEMCHECKER_H

