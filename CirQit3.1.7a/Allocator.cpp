
#include "Allocator.h"

Allocator::Allocator()
{
    createNewBlock();
}


Allocator::~Allocator()
{
    vector<void*>::iterator i;
    for( i = block_list.begin(); i != block_list.end(); i++ ) {
        free (*i);
    }
}


void * Allocator::alloc( size_t size )
{
    //cout << "****\t\t\tAllocating " << size << "\t\t\t****\n";
    void * retval = NULL;
    
    if( size > BLOCK_SIZE ) {
        assert( false );
    } else if( size < amt_free ) {
        // we can fit it in this block
        retval = (void*)((long)mem_block + BLOCK_SIZE - amt_free);
        amt_free -= size;
    } else {
        createNewBlock();
        retval = alloc( size );
    }
    return retval;
}

void Allocator::createNewBlock()
{
    mem_block = mallocC( BLOCK_SIZE );
    memset( mem_block, 0, BLOCK_SIZE );
    amt_free = BLOCK_SIZE;
    block_list.push_back( mem_block );
}

void* operator new( size_t size, Allocator * allocator )
{
    return allocator->alloc( size );
};

void* operator new[]( size_t size, Allocator * allocator )
{
    return allocator->alloc( size );
};
