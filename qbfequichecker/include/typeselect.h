/**
 * @file typeselect.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef TYPESELECT_H
#define TYPESELECT_H


namespace QBFEquiChecker_NS
{


/**
 * BiggerType<int, char>::T will return int, i.e. the bigger type
 */
template <typename LHS, typename RHS>
class BiggerType
{
    private:
        template <bool LHS_is_smaller, typename dummy = void>
        struct BiggerType_
        {
            typedef RHS T;
        };

    public:
        /// Will return the larger type between LHS and RHS
        typedef typename BiggerType_<sizeof(LHS) < sizeof(RHS)>::T T;
};


template <typename LHS, typename RHS>
template <typename dummy>
struct BiggerType<LHS, RHS>::BiggerType_<false, dummy>
{
    typedef LHS T;
};



} // namespace QBFEquiChecker_NS


#endif // TYPESELECT_H
