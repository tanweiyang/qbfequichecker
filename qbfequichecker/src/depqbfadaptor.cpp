/**
 * @file depqbfadaptor.cpp
 * @author Wei Yang Tan
 * @date 4 Dec 2012
 */


#include "depqbfadaptor.h"
#include "qbfequichecker_common.h"
#include "formulanode.h"

#ifdef USE_SOLVER_DEPQBF

extern "C"
{
#include "qdpll.h"
#include "qdpll_app.c"
}

#include <cstring>
#include <iostream>
#include <cstdio>


namespace QBFEquiChecker_NS
{

void print_result(const QDPLLResult& result)
{
    switch (result)
    {
        case QDPLL_RESULT_UNKNOWN:
            std::cout << "QDPLL_RESULT_UNKNOWN" << std::endl;
            break;
        case QDPLL_RESULT_SAT:
            std::cout << "QDPLL_RESULT_SAT" << std::endl;
            break;
        case QDPLL_RESULT_UNSAT:
            std::cout << "QDPLL_RESULT_UNSATISFIABLE" << std::endl;
            break;
    }
}


SolverResult_Type convertResult(const QDPLLResult& result)
{
    switch (result)
    {
        case QDPLL_RESULT_UNKNOWN:
            return SOLVER_UNKNOWN;
        case QDPLL_RESULT_SAT:
            return SOLVER_SAT;
        case QDPLL_RESULT_UNSAT:
            return SOLVER_UNSAT;
        default:
            return SOLVER_UNKNOWN;
    }
}


void getVarValues(QDPLL *qdpll,
                  const Size_Type& numOfVarNodes,
                  std::map<Index_t, Logic_t>& varValues)
{
    for(LitID idx = 1; idx <= numOfVarNodes; ++idx)
    {
        QDPLLAssignment value = qdpll_get_value(qdpll, idx);

        switch(value)
        {
            case QDPLL_ASSIGNMENT_FALSE:
                varValues.insert(std::make_pair(idx, Logic_t::FALSE));
                break;
            case QDPLL_ASSIGNMENT_TRUE:
                varValues.insert(std::make_pair(idx, Logic_t::TRUE));
                break;
            case QDPLL_ASSIGNMENT_UNDEF:
                varValues.insert(std::make_pair(idx, Logic_t::SYMBOLIC));
                break;
        }
    }

}


struct DepQBFAdaptor::PImpl_
{
};



DepQBFAdaptor::DepQBFAdaptor() : pImpl_(new PImpl_)
{
}


DepQBFAdaptor::~DepQBFAdaptor()
{
    delete pImpl_;
}


void DepQBFAdaptor::solve(const char *dimacs_file)
{
    FILE *input_file = fopen(dimacs_file, "r");

    if(!input_file)
        return;

    QDPLLResult result = QDPLL_RESULT_UNKNOWN;
    QDPLLApp app;
    memset(&app, 0, sizeof (QDPLLApp));
    set_default_options(&app);

    qdpll = qdpll_create();

    parse(&app, qdpll, input_file, app.options.trace);

    result = qdpll_sat(qdpll);
    print_result(result);

    cleanup(qdpll, &app);
    fclose(input_file);
 }


SolverResult_Type DepQBFAdaptor::solve(
        const FormulaNode& formulaNode,
        const std::list< std::pair<Index_t, Quantifier_Type> >& quantVarList,
        const Size_Type& numOfVarNodes,
        std::map<Index_t, Logic_t>& varValues)
{
    // if have not converted to CNF, don't continue
    if(formulaNode.getNodeType() != CNF_NODE)
        return SOLVER_UNKNOWN;

    QDPLL *qdpll = qdpll_create();

    std::list< std::pair<Index_t, Quantifier_Type> >::const_iterator ITER_END =
            quantVarList.end();
    std::list< std::pair<Index_t, Quantifier_Type> >::const_iterator iter =
            quantVarList.begin();

    Quantifier_Type prev_quant = iter->second;

    if(prev_quant == EXISTS)
    {
        qdpll_new_scope (qdpll, QDPLL_QTYPE_EXISTS);
    }
    else
    {
        qdpll_new_scope (qdpll, QDPLL_QTYPE_FORALL);
    }
    qdpll_add (qdpll, iter->first);

    for(++iter; iter != ITER_END; ++iter)
    {
        if(iter->second != prev_quant)
        {
            if(iter->second == EXISTS)
            {
                qdpll_add (qdpll, 0);
                qdpll_new_scope (qdpll, QDPLL_QTYPE_EXISTS);
            }
            else
            {
                qdpll_add (qdpll, 0);
                qdpll_new_scope (qdpll, QDPLL_QTYPE_FORALL);
            }
        }

        qdpll_add (qdpll, iter->first);
        prev_quant = iter->second;
    }

    qdpll_add (qdpll, 0);


    for(Size_Type i = 0; i < formulaNode.getNumOfChildren(); ++i)
    {
        const FormulaNode *clause = formulaNode.getChild(i);

        for(Size_Type j = 0; j < clause->getNumOfChildren(); ++j)
        {
            const FormulaNode *literalNode = clause->getChild(j);

            if(literalNode->getNodeType() == OP_NOT_NODE)
            {
                LitID idx = literalNode->getChild(0)->getIndex();
                idx = -idx;

                qdpll_add(qdpll, idx);
            }
            else
            {
                qdpll_add(qdpll, literalNode->getIndex());
            }
        }

        qdpll_add(qdpll, 0);
    }

    QDPLLResult q_result = qdpll_sat(qdpll);
    SolverResult_Type result = convertResult(q_result);

    getVarValues(qdpll, numOfVarNodes, varValues);

    qdpll_delete (qdpll);

    return result;

}

} // QBFEquiChecker_NS

#endif //USE_SOLVER_DEPQBF
