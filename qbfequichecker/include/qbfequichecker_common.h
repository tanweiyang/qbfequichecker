/**
 * @file qbfequichecker_common.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef QBFEQUICHECKER_COMMON_H
#define QBFEQUICHECKER_COMMON_H


//#include "typeselect.h"
#include "formulaparserexception.h"
#include <cstddef> // for size_t
#include <cassert>
#include <limits>


namespace QBFEquiChecker_NS
{

/// Data type used for the operand data storage
typedef int Index_t;


/// Data type for size
typedef size_t Size_Type;


enum Node_Type
{
    // using FALSE_NODE integer value as index
    FALSE_NODE = 0,
    TRUE_NODE,
    OP_NOT_NODE,
    OP_OR_NODE,
    OP_AND_NODE,
    OP_XOR_NODE,
    VAR_NODE,
    CLAUSE_NODE,
    CNF_NODE
};


enum Op_t
{
    OP_NOT = 0,
    OP_AND,
    OP_XOR,
    OP_OR,
    NUM_OF_OPERATORS
};


enum Quantifier_Type
{
    FOR_ALL,
    EXISTS
};


enum SolverResult_Type
{
    SOLVER_UNKNOWN,
    SOLVER_SAT,
    SOLVER_UNSAT,
    SOLVER_ERROR
};


static const char OPERATOR_CHARS[NUM_OF_OPERATORS] = {'~', '&', '^', '|'};


static const char WIRE_SYM = '@';
static const char WIRE_LABEL[3] = {WIRE_SYM, 'w', '\0'};


template <typename T>
void increment(T& x)
{
    if(x >= std::numeric_limits<Index_t>::max())
        throw FormulaParserException("Incrementing exceeds limit!");

    ++x;
}


/**
 * Check whether char x is an operator character
 */
bool isOperator(char x);


/**
 * Check if char x is a valid variable name
 */
bool isVarName(char x);


/**
 * @param result - the result of the solver
 * @return the string representation of the solver
 */
const char *solverResult2Str(const SolverResult_Type& result);


} // namespace QBFEquiChecker_NS


/**
 * Define the following macros to compile the solver adaptor codes
 */
//@{
#define USE_SOLVER_DEPQBF
//#define USE_SOLVER_CIRQIT
//#define USE_SOLVER_QUANTOR
//@}


#endif // QBFEQUICHECKER_COMMON_H

