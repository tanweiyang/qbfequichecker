/**
 * @file cirqitadaptor.h
 * @date Dec 1 2012
 * @author Wei Yang Tan
 *
 */

#ifndef CIRQITADAPTOR_H
#define CIRQITADAPTOR_H


#include "qbfequichecker_common.h"
#include "logic_t.h"
#include <list>
#include <utility>
#include <map>


namespace QBFEquiChecker_NS
{
class FormulaNode;

class CirQitAdaptor
{
    public:
        static const char *ISCAS_FILE;

    protected:
        void solve(const char *iscas_file = ISCAS_FILE);


        /**
         * Won't be calling the APIs of CirQit except dumping ISCAS and
         * calling it to read in ISCAS file.
         *
         * @param formulaNode
         * @param uniQuantVarVec
         * @param numOfVarNodes
         */
        SolverResult_Type solve(
                const FormulaNode& formulaNode,
                const std::list< std::pair<Index_t, Quantifier_Type> >& quantVarList,
                const Size_Type& numOfVarNodes,
                std::map<Index_t, Logic_t>& varValues);


        CirQitAdaptor();
        ~CirQitAdaptor();


    private:
        struct PImpl_;
        PImpl_ *pImpl_;
};


} // QBFEquiChecker_NS


#endif //CIRQITADAPTOR_H
