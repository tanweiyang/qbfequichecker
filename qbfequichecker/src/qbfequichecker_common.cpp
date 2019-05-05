/**
 * @file qbfequichecker_common.cpp
 *
 * @date Nov 28, 2012
 * @author Wei Yang Tan
 */

#include "qbfequichecker_common.h"


namespace QBFEquiChecker_NS
{

bool isOperator(char x)
{
    for(int i = 0; i < NUM_OF_OPERATORS; ++i)
    {
        if(OPERATOR_CHARS[i] == x)
            return true;
    }

    return false;
}


bool isVarName(char x)
{
    if(isprint(x) && x != '(' && x != ')' &&
       !isspace(x) && !isOperator(x) && x != WIRE_SYM)
        return true;

    return false;
}


const char *solverResult2Str(const SolverResult_Type& result)
{
    switch (result)
    {
        case SOLVER_UNKNOWN:
            return "Solver's result: unknown";
        case SOLVER_SAT:
            return "Solver's result: SAT";
        case SOLVER_UNSAT:
            return "Solver's result: UNSAT";
        case SOLVER_ERROR:
            return "Solver's result: ERROR!";
        default:
            return "Solver's result: ???";
    }
}


} // QBFEquiChecker_NS
