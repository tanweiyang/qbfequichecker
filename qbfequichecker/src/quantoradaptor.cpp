/**
 * @file quantoradaptor.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#include "quantoradaptor.h"
#include "qbfequichecker_common.h"
#include "formulanode.h"


#ifdef USE_SOLVER_QUANTOR


extern "C"
{
#include "quantor.h"
}

#include <cstring>
#include <iostream>


namespace QBFEquiChecker_NS
{

void print_int(const int *p)
{
    std::cout << "print_int: ";
    for(; *p != 0; ++p)
    {
        std::cout << *p << " ";
    }

    std::cout << std::endl;
}


void print_result(const QuantorResult& result)
{
    switch (result)
    {
        case QUANTOR_RESULT_UNKNOWN:
            std::cout << "QUANTOR_RESULT_UNKNOWN" << std::endl;
            break;
        case QUANTOR_RESULT_SATISFIABLE:
            std::cout << "QUANTOR_RESULT_SATISFIABLE" << std::endl;
            break;
        case QUANTOR_RESULT_UNSATISFIABLE:
            std::cout << "QUANTOR_RESULT_UNSATISFIABLE" << std::endl;
            break;
        case QUANTOR_RESULT_TIMEOUT:
            std::cout << "QUANTOR_RESULT_TIMEOUT" << std::endl;
            break;
        case QUANTOR_RESULT_SPACEOUT:
            std::cout << "QUANTOR_RESULT_SPACEOUT" << std::endl;
            break;
    }
}


SolverResult_Type convertResult(const QuantorResult& result)
{
    switch (result)
    {
        case QUANTOR_RESULT_UNKNOWN:
            return SOLVER_UNKNOWN;
        case QUANTOR_RESULT_SATISFIABLE:
            return SOLVER_SAT;
        case QUANTOR_RESULT_UNSATISFIABLE:
            return SOLVER_UNSAT;
        case QUANTOR_RESULT_TIMEOUT:
            return SOLVER_ERROR;
        case QUANTOR_RESULT_SPACEOUT:
            return SOLVER_ERROR;
        default:
            return SOLVER_UNKNOWN;
    }
}


void print_deref(Quantor *quantor)
{
    std::cout << "print_deref: ";

    int i = 1;
    int x = -1;
    do
    {
        x = quantor_deref (quantor, i);
        std::cout << "index: " << i << " x: " << x << std::endl;
        ++i;
    } while(x != -1);
}


void getVarValues(Quantor *quantor,
                  std::map<Index_t, Logic_t>& varValues)
{
    Index_t i = 1;
    int x = -1;
    x = quantor_deref(quantor, i);

    while(x != -1)
    {
        if(x == 1)
            varValues.insert(std::make_pair(i, Logic_t::TRUE));
        else if(x == 0)
            varValues.insert(std::make_pair(i, Logic_t::FALSE));

        ++i;
        x = quantor_deref(quantor, i);
    }
}


struct QuantorAdaptor::PImpl_
{
};



QuantorAdaptor::QuantorAdaptor() : pImpl_(new PImpl_)
{
}


QuantorAdaptor::~QuantorAdaptor()
{
    delete pImpl_;
}


void QuantorAdaptor::solve(const char *dimacs_file)
{
    char *outarg[2];
    outarg[0] = new char[20];
    outarg[1] = new char[20];

    strncpy(outarg[0], "a.out", 20);
    strncpy(outarg[1], dimacs_file, 20);

    quantor_main(2, outarg);
}


SolverResult_Type QuantorAdaptor::solve(
        const FormulaNode& formulaNode,
        const std::list< std::pair<Index_t, Quantifier_Type> >& quantVarList,
        const Size_Type&,
        std::map<Index_t, Logic_t>& varValues)
{
    // if have not converted to CNF, don't continue
    if(formulaNode.getNodeType() != CNF_NODE)
        return SOLVER_UNKNOWN;

    Quantor *quantor = quantor_new();


    std::list< std::pair<Index_t, Quantifier_Type> >::const_iterator ITER_END = quantVarList.end();
    std::list< std::pair<Index_t, Quantifier_Type> >::const_iterator iter = quantVarList.begin();
    Quantifier_Type prev_quant = iter->second;

    if(prev_quant == EXISTS)
    {
        quantor_scope(quantor, QUANTOR_EXISTENTIAL_VARIABLE_TYPE);
    }
    else
    {
        quantor_scope(quantor, QUANTOR_UNIVERSAL_VARIABLE_TYPE);
    }

    quantor_add(quantor, iter->first);

    for(++iter; iter != ITER_END; ++iter)
    {
        if(iter->second != prev_quant)
        {
            if(iter->second == EXISTS)
            {
                quantor_add(quantor, 0);
                quantor_scope(quantor, QUANTOR_EXISTENTIAL_VARIABLE_TYPE);
            }
            else
            {
                quantor_add(quantor, 0);
                quantor_scope(quantor, QUANTOR_UNIVERSAL_VARIABLE_TYPE);
            }
        }

        quantor_add(quantor, iter->first);
        prev_quant = iter->second;
    }

    quantor_add(quantor, 0);


    for(Size_Type i = 0; i < formulaNode.getNumOfChildren(); ++i)
    {
        const FormulaNode *clause = formulaNode.getChild(i);

        for(Size_Type j = 0; j < clause->getNumOfChildren(); ++j)
        {
            const FormulaNode *literalNode = clause->getChild(j);

            if(literalNode->getNodeType() == OP_NOT_NODE)
            {
                int idx = literalNode->getChild(0)->getIndex();
                idx = -idx;

                quantor_add(quantor, idx);
            }
            else
            {
                quantor_add(quantor, literalNode->getIndex());
            }
        }

        quantor_add(quantor, 0);
    }

    QuantorResult q_result = quantor_sat(quantor);
    SolverResult_Type result = convertResult(q_result);

    print_result(q_result);
    print_int(quantor_assignment(quantor));
    print_deref(quantor);
    getVarValues(quantor, varValues);

    //quantor_set_log(quantor, stdout);
    //quantor_print(quantor, stdout);
    //quantor_stats(quantor, stdout);

    quantor_delete(quantor);

    return result;
}

} // QBFEquiChecker_NS


#endif // USE_SOLVER_QUANTOR

