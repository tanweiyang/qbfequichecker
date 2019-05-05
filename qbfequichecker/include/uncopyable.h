/**
 * @file uncopyable.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef UNCOPYABLE_H
#define UNCOPYABLE_H


namespace QBFEquiChecker_NS
{

/**
 * @class Uncopyable
 *
 * Inherit this class privately to make your class uncopyable
 * (i.e. cannot assign or use copy constructor)
 */
class Uncopyable
{
    protected:
        Uncopyable(){}
        ~Uncopyable(){};

    private:
        Uncopyable(const Uncopyable&);
        Uncopyable& operator=(const Uncopyable&);
};

} // namespace QBFEquiChecker_NS

#endif // UNCOPYABLE_H
