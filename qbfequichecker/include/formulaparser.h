/**
 * @file formulaparser.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef FORMULAPARSER_H
#define FORMULAPARSER_H

#include "qbfequichecker_common.h"
#include "uncopyable.h"
#include <string>


namespace QBFEquiChecker_NS
{

class FormulaNode;
class FormulaNodeFactory;


/**
 * @class FormulaParser
 *
 * This class will parse in the boolean formula. A variable is identified as a token that does not 
 * consist of any character found in OPERATOR_CHARS in qbfequichecker_common.h file, and does not 
 * consist of a whitespace character.
 *
 * If the label is named "1", "0", "true" or "false", a fixed boolean variablenode will be created instead
 */
class FormulaParser : private Uncopyable
{
    public:
        explicit FormulaParser(FormulaNodeFactory& formulaNodeFactory);
        ~FormulaParser();

        /**
         * @return the root node of the parse tree
         *
         * @throw FormulaParserException if the expression is malformed
         */
        FormulaNode *parse(const std::string& exprStr);        


    private:
        struct FormulaParser_pImpl_;
        FormulaParser_pImpl_  *pImpl_;
};


} // namespace QBFEquiChecker_NS


#endif // FORMULAPARSER_H

