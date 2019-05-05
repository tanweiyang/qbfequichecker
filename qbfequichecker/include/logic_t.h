/**
 * @file logic_t.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef LOGIC_T_H
#define LOGIC_T_H


#include "qbfequichecker_common.h"
#include <algorithm>
#include <iosfwd>


namespace QBFEquiChecker_NS
{

class Logic_t
{
    public:
        enum LOGIC_TYPE_E_
        {
//            UNKNOWN = -1,
            FALSE = 0,  
            TRUE = 1,
            SYMBOLIC
        };


        explicit Logic_t(const LOGIC_TYPE_E_& value);
        explicit Logic_t(bool value);
        Logic_t(const Logic_t& rhs);
    

        void swap(Logic_t& rhs);


        void print(std::ostream& outStrm) const;
        

        inline LOGIC_TYPE_E_ getValue() const
        {
            return value_;
        }


        bool isSymbolic() const;


        // operator overloading
        Logic_t& operator=(bool rhs);
        Logic_t& operator=(const Logic_t::LOGIC_TYPE_E_& rhs);
        Logic_t& operator=(const Logic_t& rhs);
        Logic_t& operator|=(bool rhs);
        Logic_t& operator&=(bool rhs);
        Logic_t& operator^=(bool rhs);
        Logic_t& operator|=(const Logic_t::LOGIC_TYPE_E_& rhs);
        Logic_t& operator&=(const Logic_t::LOGIC_TYPE_E_& rhs);
        Logic_t& operator^=(const Logic_t::LOGIC_TYPE_E_& rhs);
        Logic_t& operator|=(const Logic_t& rhs);
        Logic_t& operator&=(const Logic_t& rhs);
        Logic_t& operator^=(const Logic_t& rhs);
        Logic_t operator~() const;
        

    private:
        /// prevent automatic conversion
        template<typename T>
        operator T () const;
        
        LOGIC_TYPE_E_ value_;
};


} // namespace QBFEquiChecker_NS


QBFEquiChecker_NS::Logic_t operator|(bool lhs, const QBFEquiChecker_NS::Logic_t& rhs);
QBFEquiChecker_NS::Logic_t operator&(bool lhs, const QBFEquiChecker_NS::Logic_t& rhs);
QBFEquiChecker_NS::Logic_t operator^(bool lhs, const QBFEquiChecker_NS::Logic_t& rhs);
QBFEquiChecker_NS::Logic_t operator|(const QBFEquiChecker_NS::Logic_t& lhs, bool rhs);
QBFEquiChecker_NS::Logic_t operator&(const QBFEquiChecker_NS::Logic_t& lhs, bool rhs);
QBFEquiChecker_NS::Logic_t operator^(const QBFEquiChecker_NS::Logic_t& lhs, bool rhs);
QBFEquiChecker_NS::Logic_t operator|(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& rhs);
QBFEquiChecker_NS::Logic_t operator&(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& rhs);
QBFEquiChecker_NS::Logic_t operator^(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& rhs);
QBFEquiChecker_NS::Logic_t operator|(const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& lhs, const QBFEquiChecker_NS::Logic_t& rhs);
QBFEquiChecker_NS::Logic_t operator&(const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& lhs, const QBFEquiChecker_NS::Logic_t& rhs);
QBFEquiChecker_NS::Logic_t operator^(const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& lhs, const QBFEquiChecker_NS::Logic_t& rhs);
QBFEquiChecker_NS::Logic_t operator|(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t& rhs);
QBFEquiChecker_NS::Logic_t operator&(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t& rhs);
QBFEquiChecker_NS::Logic_t operator^(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t& rhs);

bool operator==(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t& rhs);
bool operator==(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& rhs);
bool operator==(const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& lhs, const QBFEquiChecker_NS::Logic_t& rhs);
bool operator==(const QBFEquiChecker_NS::Logic_t& lhs, bool rhs);
bool operator==(bool lhs, const QBFEquiChecker_NS::Logic_t& rhs);
bool operator!=(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t& rhs);
bool operator!=(const QBFEquiChecker_NS::Logic_t& lhs, const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& rhs);
bool operator!=(const QBFEquiChecker_NS::Logic_t::LOGIC_TYPE_E_& lhs, const QBFEquiChecker_NS::Logic_t& rhs);
bool operator!=(const QBFEquiChecker_NS::Logic_t& lhs, bool rhs);
bool operator!=(bool lhs, const QBFEquiChecker_NS::Logic_t& rhs);


std::ostream& operator<<(std::ostream& outStrm,
                         const QBFEquiChecker_NS::Logic_t& output);


namespace std
{
    template<>
    void swap<QBFEquiChecker_NS::Logic_t>(
              QBFEquiChecker_NS::Logic_t& lhs, 
              QBFEquiChecker_NS::Logic_t& rhs);
}


#endif // LOGIC_T_H

