/**
 * @file rcptr.cpp
 * @author Wei Yang Tan
 * @date 24 Nov 2012
 */

#include "rcptr.h"
#include <cassert>


namespace QBFEquiChecker_NS
{

RCObj::RCObj() : counter_(0)
{
}


void RCObj::addReference()
{
    increment(counter_);
}


void RCObj::removeReference()
{
    assert(counter_ > 0);
    --counter_;
}

} // namespace QBFEquiChecker_NS
