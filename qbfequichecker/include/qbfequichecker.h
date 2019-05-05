/**
 * @file qbfequichecker.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef QBFEQUICHECKER_H
#define QBFEQUICHECKER_H


#include "qbfequichecker_common.h"
#include "uncopyable.h"
#include "logic_t.h"
#include <string>
#include <iosfwd>


namespace QBFEquiChecker_NS
{

class SolverAdaptorBase;


/**
 * @class QBFEquiChecker
 * 
 * The main class
 */
class QBFEquiChecker : private Uncopyable
{
    public:
        static const char *NIL_FORMULA;


        QBFEquiChecker();
        ~QBFEquiChecker();


        //@{
        /**
         * Add the index of the variable as a universal quantifier as the
         * outer quantifier
         */
        bool moveToOuterUniQuantifier(const Index_t& index);
        bool moveToOuterUniQuantifier(const std::string& varName);
        //@}


        //@{
        /**
         * Add the index of the variable as a extential quantifier as the
         * outer quantifier
         */
        bool moveToOuterExtQuantifier(const Index_t& index);
        bool moveToOuterExtQuantifier(const std::string& varName);
        //@}


        //@{
        /**
         * Add the index of the variable as a universal quantifier as the
         * inner quantifier
         */
        bool moveToInnerUniQuantifier(const Index_t& index);
        bool moveToInnerUniQuantifier(const std::string& varName);
        //@}


        //@{
        /**
         * Add the index of the variable as a extential quantifier as the
         * inner quantifier
         */
        bool moveToInnerExtQuantifier(const Index_t& index);
        bool moveToInnerExtQuantifier(const std::string& varName);
        //@}


        /**
         * Remove all the variables attached to the universal quantifier and
         * reset to existential
         */
        void resetQuantifier();


        /**
         * Create a miter
         *
         * @param formula1
         * @param formula2
         */
        //void createMiter(const std::string& formula1,
        //                 const std::string& formula2);



        /**
         * Parse the formula. All variables will be attached as existential
         * quantifier
         *
         * @param exprStr - boolean formula
         * @return true if no error
         */
        bool parse(const std::string& exprStr);

    
        /**
         * @return The corresponding variable label from its index
         */
        void printIndexVarMap(std::ostream& outstream) const;
    
    
        /**
         * @return Number of variables
         */
        Size_Type getNumOfVar() const;


        /**
         * @param index - Index of the variable
         * @return The variable name
         */
        std::string getVarName(const Index_t& index) const;


        /**
         * Get the variable index given the name
         * @param varName - Name of the variable
         * @return The corresponding index
         */
        Index_t getVarIndex(const std::string& varName) const;

    
        /**
         * Convert to a CNF representation
         *
         * @return false if already converted to CNF or ISCAS or formula not parsed yet;
         *         true otherwise
         */
        bool convertCNF();


        /**
         * Convert to ISCAS representation
         *
         * @return false if already converted to CNF or ISCAS or formula not parsed yet;
         *         true otherwise
         */
        std::string convertISCAS();


        /**
         * @return get ISCAS string
         */
        //std::string getISCAS() const;

    
        /**
         * Simplify the expression
         */
        void simplify();
    
    
        /**
         * Print out the formula
         */
        void printFormula(std::ostream& outStrm) const;
    
    
        /**
         * Print out the equivalent CNF
         */
        void printCNF(std::ostream& outStrm) const;
    
    
        /**
         * Print out the CNF
         */
        void printDIMACS(std::ostream& outStrm) const;


        /**
         * Solve the QBF problem. For the no parameter function, we will be
         * using Quantor as the default solver
         *
         * @param solver - the solver that you want to use; will invoke
         *                 solver.solve(FormulaNode&, std::vector<Index_t>, int)
         * @param testFile - The input file for checking;
         *                   will invoke solver.solve(testFile)
         */
        //@{

#ifdef USE_SOLVER_DEPQBF
        SolverResult_Type solve() const;
#endif //USE_SOLVER_DEPQBF

        SolverResult_Type solve(SolverAdaptorBase& solver) const;
        void solve(SolverAdaptorBase& solver, const char *testFile) const;
        //@}


        /**
         *
         * @return time taken to solve (in seconds)
         */
        double timeTakenForSolving() const;


        /**
         * Print the assignments of the variables after solving
         * @param outstream
         */
        void printVarAssignments(std::ostream& outstream) const;


        /**
         * Get the value of the variable assignment after solving. Note that
         * if the result is UNSAT or if you have not invoked solve(), the
         * return result may be garbage (depending on your solver)
         *
         * @param index
         * @return The value of the variable
         */
        Logic_t getVarAssignment(const Index_t& index) const;


        /// for debugging
        //@{
        unsigned int totalNumOfNodes() const;
        bool setVariableValue(Index_t index, const Logic_t& val);
        bool setVariableValue(Index_t index, const Logic_t::LOGIC_TYPE_E_& val);
        bool setVariableValue(Index_t index, bool val);
        Logic_t evaluate();
        //@}


    private:
        struct PImpl_;
        PImpl_ *pImpl_;
};

}


#endif // QBFEQUICHECKER_H

