/**
 * @file solveradaptorbase.h
 * @author Wei Yang Tan
 * @date 30 Nov 2012
 *
 */
#ifndef SOLVERADAPTORBASE_H
#define SOLVERADAPTORBASE_H


#include "qbfequichecker_common.h"
#include "logic_t.h"
#include <list>
#include <utility>
#include <map>


namespace QBFEquiChecker_NS
{

class FormulaNode;

/**
 * @class SolverAdaptorBase
 *
 * The base class for SolverAdaptor template class
 */
class SolverAdaptorBase
{
    public:
        virtual ~SolverAdaptorBase() = 0;


        /**
         * Using this solver to solve the SAT problem.
         *
         * @param formulaNode
         * @param quantVarList
         * @param numOfVarNodes
         * @param varValues - the result of the variable assignment if SAT
         * @return the results
         */
        virtual SolverResult_Type solve(
                const FormulaNode& formulaNode,
                const std::list< std::pair<Index_t, Quantifier_Type> >& quantVarList,
                const Size_Type& numOfVarNodes,
                std::map<Index_t, Logic_t>& varValues) = 0;


        /**
         * Using the solver to solve the SAT problem; problem itself is
         * formulated as some qdimacs file or iscas file format, depending on
         * which solver you are using. The results is currently printed
         * at stdout / cout stream.
         *
         * @param inputFile
         */
        virtual void solve(const char *inputFile) = 0;

};


} // namespace QBFEquiChecker_NS


#endif // SOLVERADAPTORBASE_H
