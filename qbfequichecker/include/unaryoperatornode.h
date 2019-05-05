/**
 * @file unaryoperatornode.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef UNARYOPERATORNODE_H
#define UNARYOPERATORNODE_H


#include "operatornode.h"
#include "logic_t.h"
#include "qbfequichecker_common.h"
#include <iosfwd>
#include <vector>
#include <string>

namespace QBFEquiChecker_NS
{

/**
 * @class UnaryOperatorNode
 *
 * Parent class for all unary operator nodes
 */
class UnaryOperatorNode : public OperatorNode
{
    public:
        /**
         * @note The child node will not be cloned
         */
        UnaryOperatorNode(FormulaNodeFactory& formulaNodeFactory,
                          FormulaNode& operand,
                          const Index_t& index,
                          const std::string& label);

        /**
         * @note The child node will not be deleted
         */
		virtual ~UnaryOperatorNode();

		
        virtual UnaryOperatorNode *clone() const = 0;


    protected:
		void swap(UnaryOperatorNode& rhs);

        /**
         *  @note copy constructor will not clone the child nodes
         */ 
        UnaryOperatorNode(const UnaryOperatorNode& rhs);
        //UnaryOperatorNode& operator=(const UnaryOperatorNode& rhs);


    private:
        virtual Logic_t evaluate_() = 0;
        virtual void print_(std::ostream& outStrm) const = 0;
        virtual std::string findLabel_(const Index_t& index) const;
        virtual Index_t findVarIndex_(const std::string& varName) const;
        virtual Size_Type getTreeHeight_() const;
        virtual bool setVariableVal_(const Index_t& index, const Logic_t& value);
        virtual FormulaNode *getChild_(unsigned int childIndex); 
        virtual bool setChild_(FormulaNode& childNode, unsigned int index);
        virtual const FormulaNode *getChild_(unsigned int childIndex) const;
        virtual Size_Type getNumOfChildren_() const;
        virtual FormulaNode& simplify_() = 0;
        virtual void removeAllChildrenNode_();
        virtual bool removeChildNode_(unsigned int childIndex);
        virtual void deleteTree_();
        virtual FormulaNode& convertCNF_(FormulaNode& cnfNode) = 0;
        virtual FormulaNode& convertISCAS_(std::string& outStr) = 0;

        FormulaNode *operand_;
};


} // namespace QBFEquiChecker_NS


#endif // UNARYOPERATORNODE_H

