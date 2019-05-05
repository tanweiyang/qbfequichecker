/**
 * @file quantoradaptor.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef QUANTORADAPTOR_H
#define QUANTORADAPTOR_H


#include "qbfequichecker_common.h"
#include "logic_t.h"
#include <list>
#include <utility>
#include <map>


namespace QBFEquiChecker_NS
{

class FormulaNode;

/**
 * @class QuantorAdaptor
 *
 * A policy class for the SolverAdaptor. Use SolverAdaptor as the template wrapper to use this class
 */
class QuantorAdaptor
{
    protected:
        void solve(const char *dimacs_file);
        SolverResult_Type solve(
                const FormulaNode& formulaNode,
                const std::list< std::pair<Index_t, Quantifier_Type> >& quantVarList,
                const Size_Type& numOfVarNodes,
                std::map<Index_t, Logic_t>& varValues);


        QuantorAdaptor();
        ~QuantorAdaptor();


    private:
        struct PImpl_;
        PImpl_ *pImpl_;
};


} // namespace QBFEquiChecker_NS


#endif // QUANTORADAPTOR_H


