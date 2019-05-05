#include "memchecker.h"

//#ifdef DEBUG_MEM_LEAKS 

#include <iostream>


void atExitFn();


namespace memchecker_NS
{

struct InitializeAtExitFn
{
    InitializeAtExitFn()
    {
        atexit(atExitFn);
    }
};


int mem_counter_ = 0;
int mem_array_counter_ = 0;

}


static memchecker_NS::InitializeAtExitFn initializeAtExitFn_;


void atExitFn()
{
    if(memchecker_NS::mem_counter_ > 0)
    {
        std::cerr << "MEMORY LEAK!!! You have \"new\" more than you \"delete" 
                  << std::endl;
    }
    else if(memchecker_NS::mem_counter_ > 0)
    {
        std::cerr << "MULTIPLE DEALLOCATION OF MEMORY!!! "
                  << "You have \"delete\" more than you \"new" 
                  << std::endl;
    }

    if(memchecker_NS::mem_array_counter_ > 0)
    {
        std::cerr << "MEMORY LEAK!!! "
                  << "You have \"new[]\" more than you \"delete[]" 
                  << std::endl;
    }
    else if(memchecker_NS::mem_array_counter_ < 0)
    {
        std::cerr << "MULTIPLE DEALLOCATION OF MEMORY!!! "
                  << "You have \"delete[]\" more than you \"new[]" 
                  << std::endl;
    }
}


void *operator new(size_t size)
{
    ++memchecker_NS::mem_counter_;
    return malloc(size);
}


void *operator new[](size_t size)
{
    ++memchecker_NS::mem_array_counter_;
    return malloc(size);
}


void operator delete(void *ptr)
{
    --memchecker_NS::mem_counter_;
    free(ptr);
}


void operator delete[](void *ptr)
{
    --memchecker_NS::mem_array_counter_;
    free(ptr);
}



//#endif // DEBUG_MEM_LEAKS 

