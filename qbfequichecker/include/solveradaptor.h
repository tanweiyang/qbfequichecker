/**
 * @file solveradaptor.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef SOLVERADAPTOR_H
#define SOLVERADAPTOR_H

#include "solveradaptorbase.h"
#include "qbfequichecker_common.h"
#include "logic_t.h"
#include <list>
#include <utility>
#include <map>


namespace QBFEquiChecker_NS
{

class FormulaNode;

/**
 * @class SolverAdaptor
 *
 * A policy container class for wrapping around different solver adaptor classes
 */
template<typename T>
class SolverAdaptor : public SolverAdaptorBase, private T
{
    public:
        SolverAdaptor();
        virtual ~SolverAdaptor();


        virtual SolverResult_Type solve(
                const FormulaNode& formulaNode,
                const std::list< std::pair<Index_t, Quantifier_Type> >& quantVarList,
                const Size_Type& numOfVarNodes,
                std::map<Index_t, Logic_t>& varValues);


        virtual void solve(const char *inputfile);

};



///////////////////////////////////////////////
// Class Functions Definitions
///////////////////////////////////////////////
template<typename T>
SolverAdaptor<T>::SolverAdaptor()
{
}


template<typename T>
SolverAdaptor<T>::~SolverAdaptor()
{
}


template<typename T>
SolverResult_Type SolverAdaptor<T>::solve(
        const FormulaNode& formulaNode,
        const std::list< std::pair<Index_t, Quantifier_Type> >& quantVarList,
        const Size_Type& numOfVarNodes,
        std::map<Index_t, Logic_t>& varValues)
{
    SolverResult_Type result = T::solve(formulaNode,
                                        quantVarList,
                                        numOfVarNodes,
                                        varValues);

    return result;
}


template<typename T>
void SolverAdaptor<T>::solve(const char *inputFile)
{
    T::solve(inputFile);
}


} // namespace QBFEquiChecker_NS



#endif // SOLVERADAPTOR_H

