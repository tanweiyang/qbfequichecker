/**
 * @file logic_t.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */



#include "logic_t.h"
#include <ostream>


namespace QBFEquiChecker_NS
{

Logic_t::Logic_t(const LOGIC_TYPE_E_& value) : value_(value)
{
}


Logic_t::Logic_t(bool value)
{
    if(value)
    {
        value_ = Logic_t::TRUE; 
    }
    else
    {
        value_ = Logic_t::FALSE; 
    }
}


Logic_t::Logic_t(const Logic_t& rhs) : value_(rhs.value_)
{
}


void Logic_t::swap(Logic_t& rhs)
{
    using std::swap;
    
    swap(this->value_, rhs.value_);
}


bool Logic_t::isSymbolic() const
{
    if(value_ == Logic_t::SYMBOLIC)
        return true;
    else
        return false;
}


Logic_t& Logic_t::operator=(bool rhs)
{
    Logic_t tmp(rhs);
    this->swap(tmp);

    return *this;
}
 

Logic_t& Logic_t::operator=(const Logic_t::LOGIC_TYPE_E_& rhs)
{
    value_ = rhs;
    
    return *this;
}


Logic_t& Logic_t::operator=(const Logic_t& rhs)
{
    Logic_t tmp(rhs);
    this->swap(tmp);

    return *this;
}
 

Logic_t& Logic_t::operator|=(bool rhs)
{
    Logic_t tmp(rhs);
    *this |= rhs;

    return *this;
}
 

Logic_t& Logic_t::operator&=(bool rhs)
{
    Logic_t tmp(rhs);
    *this &= tmp;

    return *this;
}


Logic_t& Logic_t::operator^=(bool rhs)
{
    Logic_t tmp(rhs);
    *this ^= tmp;

    return *this;
}


Logic_t& Logic_t::operator|=(const Logic_t::LOGIC_TYPE_E_& rhs)
{
    Logic_t tmp(rhs);
    *this &= tmp;

    return *this;
}
 

Logic_t& Logic_t::operator&=(const Logic_t::LOGIC_TYPE_E_& rhs)
{
    Logic_t tmp(rhs);
    *this &= tmp;

    return *this;
}


Logic_t& Logic_t::operator^=(const Logic_t::LOGIC_TYPE_E_& rhs)
{
    Logic_t tmp(rhs);
    *this &= tmp;

    return *this;
}


Logic_t& Logic_t::operator|=(const Logic_t& rhs)
{
    if(value_ == Logic_t::TRUE || rhs.value_ == Logic_t::TRUE)
    {
        value_ = Logic_t::TRUE;
    }
    else if(value_ == Logic_t::FALSE && rhs.value_ == Logic_t::FALSE)
    {
        value_ = Logic_t::FALSE;
    }
    else
    {
        value_ = Logic_t::SYMBOLIC;
    }

    return *this;
}
 

Logic_t& Logic_t::operator&=(const Logic_t& rhs)
{
    if(value_ == Logic_t::FALSE || rhs.value_ == Logic_t::FALSE)
    {
        value_ = Logic_t::FALSE;
    }
    else if(value_ == Logic_t::TRUE && rhs.value_ == Logic_t::TRUE)
    {
        value_ = Logic_t::TRUE;
    }
    else
    {
        value_ = Logic_t::SYMBOLIC;
    }

    return *this;
}


Logic_t& Logic_t::operator^=(const Logic_t& rhs)
{
    if((value_ == Logic_t::FALSE && rhs.value_ == Logic_t::FALSE) ||
       (value_ == Logic_t::TRUE && rhs.value_ == Logic_t::TRUE))
    {
        value_ = Logic_t::FALSE;
    }
    else if((value_ == Logic_t::FALSE && rhs.value_ == Logic_t::TRUE) ||
            (value_ == Logic_t::TRUE && rhs.value_ == Logic_t::FALSE))
    {
        value_ = Logic_t::TRUE;
    }
    else
    {
        value_ = Logic_t::SYMBOLIC;
    }

    return *this;
}


Logic_t Logic_t::operator~() const
{
    Logic_t tmp(*this);

    if(value_ == Logic_t::TRUE)
    {
        tmp.value_ = Logic_t::FALSE;
    }
    else if(value_ == Logic_t::FALSE)
    {
        tmp.value_ = Logic_t::TRUE;
    }

    return tmp;
}


void Logic_t::print(std::ostream& outStrm) const
{
    switch(value_)
    {        
//        case Logic_t::UNKNOWN:
//            outStrm << "?";
//            break;
        case Logic_t::TRUE:
            outStrm << "1";
            break; 
        case Logic_t::FALSE:
            outStrm << "0";
            break;
        case Logic_t::SYMBOLIC:
            outStrm << "X";
            break;
    }
}

} // namespace QBFEquiChecker_NS


QBFEquiChecker_NS::Logic_t operator|(bool lhs, const QBFEquiChecker_NS::Logic_t& rhs)
{
    QBFEquiChecker_NS::Logic_t tmp(lhs);
    return (tmp | rhs);
}


QBFEquiChecker_NS::Logic_t operator&(bool lhs, const QBFEquiChecker_NS::Logic_t& rhs)
{
    QBFEquiChecker_NS::Logic_t tmp(lhs);
    return (tmp & rhs);
}


QBFEquiChecker_NS::Logic_t operator^(bool lhs, const QBFEquiChecker_NS::Logic_t& rhs)
{
    QBFEquiChecker_NS::Logic_t tmp(lhs);
    return (tmp ^ rhs);
}


QBFEquiChecker_NS::Logic_t operator|(const QBFEquiChecker_NS::Logic_t& lhs, bool rhs)
{
    return (rhs | lhs);
}


QBFEquiChecker_NS::Logic_t operator&(const QBFEquiChecker_NS::Logic_t& lhs, bool rhs)
{
    return (rhs & lhs);
}


QBFEquiChecker_NS::Logic_t operator^(const QBFEquiChecker_NS::Logic_t& lhs, bool rhs)
{
    return (rhs ^ lhs);
}


QBFEquiChecker_NS::Logic_t operator|(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& rhs)
{
    QBFEquiChecker_NS::Logic_t tmp(rhs);
    return (tmp | lhs);
}


QBFEquiChecker_NS::Logic_t operator&(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& rhs)
{
    QBFEquiChecker_NS::Logic_t tmp(rhs);
    return (tmp & lhs);
}


QBFEquiChecker_NS::Logic_t operator^(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& rhs)
{
    QBFEquiChecker_NS::Logic_t tmp(rhs);
    return (tmp ^ lhs);
}


QBFEquiChecker_NS::Logic_t operator|(const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& lhs, const QBFEquiChecker_NS::Logic_t& rhs)
{
    return (rhs | lhs);
}


QBFEquiChecker_NS::Logic_t operator&(const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& lhs, const QBFEquiChecker_NS::Logic_t& rhs)
{
    return (rhs & lhs);
}


QBFEquiChecker_NS::Logic_t operator^(const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& lhs, const QBFEquiChecker_NS::Logic_t& rhs)
{
    return (rhs ^ lhs);
}


QBFEquiChecker_NS::Logic_t operator|(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t& rhs)
{
    QBFEquiChecker_NS::Logic_t tmp = lhs;
    tmp |= rhs;
    return tmp;
}


QBFEquiChecker_NS::Logic_t operator&(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t& rhs)
{
    QBFEquiChecker_NS::Logic_t tmp = lhs;
    tmp &= rhs;
    return tmp;
}


QBFEquiChecker_NS::Logic_t operator^(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t& rhs)
{
    QBFEquiChecker_NS::Logic_t tmp = lhs;
    tmp ^= rhs;
    return tmp;
}


bool operator==(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t& rhs)
{
    return lhs.getValue() == rhs.getValue();
}


bool operator==(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& rhs)
{
    return (lhs == QBFEquiChecker_NS::Logic_t(rhs));
}


bool operator==(const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& lhs, const QBFEquiChecker_NS::Logic_t& rhs)
{
    return (rhs == lhs);
}


bool operator==(const QBFEquiChecker_NS::Logic_t& lhs, bool rhs)
{
/*
    if((rhs == true && lhs.getValue() == QBFEquiChecker_NS::Logic_t::TRUE) ||
       (rhs == false && lhs.getValue() == QBFEquiChecker_NS::Logic_t::FALSE))
        return true;
    else
        return false;
*/
    return (lhs == QBFEquiChecker_NS::Logic_t(rhs));
}


bool operator==(bool lhs, const QBFEquiChecker_NS::Logic_t& rhs)
{
    return (rhs == lhs);
}


bool operator!=(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t& rhs)
{
    return !(lhs == rhs);
}


bool operator!=(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& rhs)
{
    return !(lhs == rhs);
}


bool operator!=(const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& lhs, const QBFEquiChecker_NS::Logic_t& rhs)
{
    return !(lhs == rhs);
}


bool operator!=(const QBFEquiChecker_NS::Logic_t& lhs, bool rhs)
{
    return !(lhs == rhs);
}


bool operator!=(bool lhs, const QBFEquiChecker_NS::Logic_t& rhs)
{
    return !(lhs == rhs);
}


std::ostream& operator<<(std::ostream& outStrm,
                         const QBFEquiChecker_NS::Logic_t& output)
{
    output.print(outStrm);
    return outStrm;
}


namespace std
{
    template<>
    void swap<QBFEquiChecker_NS::Logic_t>(
              QBFEquiChecker_NS::Logic_t& lhs, 
              QBFEquiChecker_NS::Logic_t& rhs)
    {
        lhs.swap(rhs);
    }
}

