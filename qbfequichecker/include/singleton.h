#ifndef SINGLETON_H
#define SINGLETON_H


#include <cassert>


namespace QBFEquiChecker_NS
{

/**
 * A template class for implementing a singleton class
 */
template <class T>
class Singleton : public T
{
    public:
        /**
         * Get the singleton pointer
         */  
        static T& instance(); 


    protected:
        Singleton(){};
        ~Singleton(){};


    private:
        Singleton(Singleton const&);
        Singleton& operator=(Singleton const&);
};
   

/// To initializs the static pointer member
//template <class T> T *Singleton<T>::p_ = NULL;
 

template <class T>
T& Singleton<T>::instance() 
{
    static T sObj_;

    return sObj_;
}

  
}

#endif // SINGLETON_H

