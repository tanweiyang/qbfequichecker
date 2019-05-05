/**
 * @file formulaparser.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#include "formulaparser.h"
#include "formulanode.h"
#include "binaryoperatornode.h"
#include "unaryoperatornode.h"
#include "operatorornode.h"
#include "operatorxornode.h"
#include "operatornotnode.h"
#include "operatorandnode.h"
#include "literalnode.h"
#include "variablenode.h"
#include "cnfnode.h"
#include "clausenode.h"
#include "truenode.h"
#include "falsenode.h"
#include "operatornode.h"
#include "formulanode.h"

#include "formulanodefactory.h"
#include "formulaparserexception.h"
#include "qbfequichecker_common.h"
#include "uncopyable.h"

//#include <memory> // for std::auto_ptr
#include <cstddef>  // for size_t
#include <cctype>
#include <cassert>
#include <limits>
#include <vector>
#include <algorithm>


#include <iostream>
template<typename T>
void printVector(const T& x)
{
    std::cout << x << " "; 
}




namespace QBFEquiChecker_NS
{

// ###############################################
// FUNCTION PROTOTYPES
// ###############################################
/*
 * @return true if x is an operator character; false otherwise
 */
bool isOperator(char x);

/*
 * @return true if x is a variable name; false otherwise
 */
bool isVarName(char x);


// ###############################################
// FormulaParser_pImpl_ DEFINITIONS
// ###############################################
struct FormulaParser::FormulaParser_pImpl_ : private Uncopyable
{
    FormulaParser_pImpl_(FormulaNodeFactory& formulaNodeFactory) :
        formulaNodeFactory_(formulaNodeFactory)
    {
    }


    ~FormulaParser_pImpl_()
    {
        formulaNodeFactory_.clear();
    }


    /*
     * Parse the expression
     */
    FormulaNode *parse(const std::string& exprStr);


    /*
     * Get a CNF representation
     */
    void convertCNF();

   
    /*
     * Simplify the formula
     */ 
    void simplify();


   /*
    * Looping function for parentheses
    */
    FormulaNode *buildExprTreeBrac(const std::string& exprStr,
                                   size_t& index);


   /*
    * Extract a node from the expression. This additional function is required
    * as it will be invoked when looking for the operand node(s) for the operator
    * nodes instead of calling the looping buildExprTree_ function
    * 
    * @throw FormulaParserException - if the node extracted is null
    */
    FormulaNode *extractNode(const std::string& exprStr, 
                             size_t& index,
                             FormulaNode *prev_operand = 0);


   /*
    * @param[inout] index - input the starting of the string position index for
    *                       processing and it will return with one more than the 
    *                       operand position.
    * 
    * @throw FormulaParserException - if data symbol is not supported
    */
    FormulaNode *createLiteralNode(const std::string& exprStr,
                                   size_t& index);


   /*
    * @param[inout] index - input the starting of the string position index for
    *                       processing and it will return with one more than the 
    *                       operator position.
    * 
    * @throw FormulaParserException - if operator symbol is not supported
    */
    FormulaNode *createOperatorNode(const std::string& exprStr, 
                                    size_t& index,
                                    FormulaNode *prev_operand);

    FormulaNodeFactory& formulaNodeFactory_;
};


// ###############################################
// CLASS FUNCTION DEFINITIONS
// ###############################################
// ------------------------
// FormulaParser
// ------------------------
FormulaParser::FormulaParser(FormulaNodeFactory& formulaNodeFactory) :
    pImpl_(NULL)
{
    pImpl_ = new FormulaParser_pImpl_(formulaNodeFactory);
}


FormulaParser::~FormulaParser()
{
    delete pImpl_;
}


FormulaNode *FormulaParser::parse(const std::string& exprStr)
{
    return pImpl_->parse(exprStr);
}


// ------------------------
// FormulaParser_pImpl_
// ------------------------
FormulaNode *FormulaParser::FormulaParser_pImpl_::parse(
        const std::string& exprStr)
{
	size_t index = 0;
    FormulaNode *root = NULL;

    // clearing the factory memory is qbfequichecker's job
    while(index < exprStr.length())
    {
		root = extractNode(exprStr, index, root);
    }

//    while(index < exprStr.length())
//    {
//        if(!isspace(exprStr[index]))
//        {
//	        throw FormulaParserException("Formula is ill-formed!");
//        }   
//        ++index;
//    }
    
    return root;
}


FormulaNode *FormulaParser::FormulaParser_pImpl_::buildExprTreeBrac(
        const std::string& exprStr,
        size_t& index)
{
    FormulaNode *prev_operand = NULL;

    while(index < exprStr.length())
    {
        // ignore white space
        for( ; index < exprStr.length() && isspace(exprStr[index]); ++index){}

        if(exprStr[index] == ')')
        {
            ++index;
            return prev_operand;
        }
        else
        {
            prev_operand = extractNode(exprStr, index, prev_operand);
        }
    }

	throw FormulaParserException("Parentheses error in the expression!");
}


FormulaNode *FormulaParser::FormulaParser_pImpl_::extractNode(
        const std::string& exprStr,
        size_t& index,
        FormulaNode *prev_operand)
{
    assert(index <= exprStr.length());

    // skip all white spaces
    for( ; index < exprStr.length() && isspace(exprStr[index]); ++index){}

    if(index == exprStr.length())
        return prev_operand;
	
	FormulaNode *extractedNode;

    char curr_char = exprStr[index];
    if(isOperator(curr_char))
    {   
		extractedNode = createOperatorNode(exprStr, index, prev_operand);
    }
    else if(curr_char == '(')
    {
		extractedNode = buildExprTreeBrac(exprStr, ++index);
        //extractedNode = extractNode(exprStr, ++index, 0);
    }
    else
    {
        if(prev_operand != NULL && prev_operand->getNodeType() == VAR_NODE)
        {
            throw FormulaParserException(
                    "VariableNode should not have a LHS operand!");
        }

		extractedNode = createLiteralNode(exprStr, index);
    }
	
	return extractedNode;
}


FormulaNode *FormulaParser::FormulaParser_pImpl_::createOperatorNode(
        const std::string& exprStr,
        size_t& index,
        FormulaNode *prev_operand)
{
    assert(index < exprStr.length());

    if(exprStr[index] == OPERATOR_CHARS[OP_OR]) // '|'
    {
        if(!prev_operand)
		    throw FormulaParserException("OR operator has no LHS operand!");

        FormulaNode *next_operand = extractNode(exprStr, ++index);
		
	    if(!next_operand)
		    throw FormulaParserException("OR operator has no RHS operand!");

        FormulaNode& opNode = formulaNodeFactory_.createOperatorOrNode(
                *prev_operand, *next_operand);

        return &opNode;
    }
    if(exprStr[index] == OPERATOR_CHARS[OP_XOR]) // '^'
    {
        if(!prev_operand)
            throw FormulaParserException("XOR operator has no LHS operand!");

        FormulaNode *next_operand = extractNode(exprStr, ++index);

        if(!next_operand)
            throw FormulaParserException("XOR operator has no RHS operand!");

        FormulaNode& opNode = formulaNodeFactory_.createOperatorXorNode(
                *prev_operand, *next_operand);

        return &opNode;
    }
    else if(exprStr[index] == OPERATOR_CHARS[OP_AND]) // '&'
    {
        if(!prev_operand)
            throw FormulaParserException("AND operator has no LHS operand!");

        FormulaNode *next_operand = extractNode(exprStr, ++index);
		
	    if(!next_operand)
            throw FormulaParserException("AND operator has no RHS operand!");
	
        FormulaNode& opNode = formulaNodeFactory_.createOperatorAndNode(
                *prev_operand, *next_operand);

        return &opNode;
    }
    else if(exprStr[index] == OPERATOR_CHARS[OP_NOT]) // '~'
    {
        if(prev_operand)
           throw FormulaParserException("NOT operator has an invalid LHS operand!");
        
      	FormulaNode *next_operand = extractNode(exprStr, ++index);
		
        if(!next_operand)
            throw FormulaParserException("NOT operator has no RHS operand!");

	
        FormulaNode& opNode = formulaNodeFactory_.createOperatorNotNode(
                *next_operand);

        return &opNode;
    }
   
    throw FormulaParserException("Operator is not supported");
}


FormulaNode *FormulaParser::FormulaParser_pImpl_::createLiteralNode(
        const std::string& exprStr,
        size_t& index)
{
    assert(index < exprStr.length());

//    for(size_t expr_len = exprStr.length(); !isVarName(exprStr[index]); ++index)
    for(size_t expr_len = exprStr.length(); isspace(exprStr[index]); ++index)
    {
        // can it be because of trailing whitespace?
        if(index >= expr_len || !isVarName(exprStr[index]))
            throw FormulaParserException("Error in creating variable node");
    }

    size_t first_index = index;
    for( ; index < exprStr.length() && isVarName(exprStr[index]); ++index){}
    size_t substrLen = index - first_index; 

    std::string varLabel = exprStr.substr(first_index, substrLen); 

    if(varLabel == "1" || varLabel == "true")
    {
        return &(formulaNodeFactory_.createTrueNode());
    } 
    else if(varLabel == "0" || varLabel == "false")
    {
        return &(formulaNodeFactory_.createFalseNode());
    }
    else
    {
        return &(formulaNodeFactory_.createVariableNode(
                varLabel, Logic_t::SYMBOLIC));
    }
}


// ###############################################
// NON-CLASS FUNCTION DEFINITIONS
// ###############################################



} // namespace QBFEquiChecker_NS

