/**
 * @file rcptr.h
 * @author Wei Yang Tan
 * @date 24 Nov 2012
 */

#ifndef RCPTR_H
#define RCPTR_H


#include "qbfequichecker_common.h"


namespace QBFEquiChecker_NS
{

/**
 * @class RCObj
 *
 * The class for implementing the counter
 */
class RCObj
{
    public:
        RCObj();
        void addReference();
        void removeReference();

        inline Size_Type getCounter() const
        {
            return counter_;
        }


    private:
        Size_Type counter_;
};


/**
 * @class RCPtr
 *
 * A simple smart pointer that will perform reference counting.
 *
 * @note My implementation is a bit different from the commonly implemented way:
 *       if the pointer held in this smart pointer is NULL, my reference
 *       counting will still increase to 1. If another smart pointer assign
 *       to another smart pointer, even if the internal pointer stored is NULL,
 *       I will still increment the reference counting. This is to make my life
 *       easier especially when considering the mentioned scenario: messy
 *       when one smart pointer with a NULL pointer inside is assigned to
 *       another one: what should the value of the reference counting be and
 *       should both still share the same reference counting object?
 */
template<class T>                      
class RCPtr 
{                         
    public: 
        explicit RCPtr(T* realPtr = 0);
        RCPtr(const RCPtr& rhs);
        ~RCPtr();
   
        // Operator overloading
        RCPtr& operator=(const RCPtr& rhs);    
        T *operator->() const;
        T& operator*() const;

        /**
         * @return The raw pointer
         */
        T *get();


        /**
         * Deletes the raw pointer and reset the counter to 1
         */
        void reset();


        /**
         * Deletes the raw pointer and takes owner of ptr
         * @param ptr The pointer which ownership will be taken over
         */
        void reset(T *ptr);

        /// For debugging: return the counter value
        Size_Type getRefCount() const;


     private:
        RCObj *rcobj_;
        T *pointee_;

        void init_();
        void decr_(T *& pointee, RCObj *&rcobj) const;
};


///////////////////////////////////////////////////////////////// 
// Template class definitions
///////////////////////////////////////////////////////////////// 
template<class T>
void RCPtr<T>::init_()
{
    rcobj_->addReference();
}


template<class T>
void RCPtr<T>::decr_(T *& pointee, RCObj *& rcobj) const
{
    rcobj->removeReference();

    if(0 == rcobj->getCounter())
    {
        delete pointee;
        delete rcobj;

        pointee = 0;
        rcobj = 0;
    }
}


template<class T>
RCPtr<T>::RCPtr(T *realPtr) : 
    rcobj_(new RCObj),
    pointee_(realPtr)
{
    init_();
}

 
template<class T>
RCPtr<T>::RCPtr(const RCPtr& rhs) : 
    rcobj_(rhs.rcobj_),
    pointee_(rhs.pointee_)
{ 
    init_();
}
 

template<class T>
RCPtr<T>::~RCPtr()
{
    decr_(this->pointee_, this->rcobj_);
}


template<class T>
RCPtr<T>& RCPtr<T>::operator=(const RCPtr& rhs)
{
    if(pointee_ != rhs.pointee_)
    {
        T *oldPointee = pointee_;
        RCObj *oldObj = rcobj_;
        pointee_ = rhs.pointee_;
        rcobj_ = rhs.rcobj_;
        init_();

        decr_(oldPointee, oldObj);
    }
   
    return *this;
}
 

template<class T>
T *RCPtr<T>::operator->() const 
{
    return pointee_;
}
 

template<class T>
T& RCPtr<T>::operator*() const
{
    return *pointee_;
}


template<class T>
T *RCPtr<T>::get()
{
    return pointee_;
}


template<class T>
void RCPtr<T>::reset()
{
    decr_(this->pointee_, this->rcobj_);
    this->pointee_ = 0;
    this->rcobj_ = new RCObj;
    init_();
}


template<class T>
void RCPtr<T>::reset(T *ptr)
{
    reset();
    this->pointee_ = ptr;
}



template<class T>
Size_Type RCPtr<T>::getRefCount() const
{
    if(!rcobj_)
        return 0;

    return rcobj_->getCounter();
}


} // namespace QBFEquiChecker_NS


#endif // RCPtr

