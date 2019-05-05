/**
 * @file binaryoperatornode.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef BINARYOPERATORNODE_H
#define BINARYOPERATORNODE_H


#include "operatornode.h"
#include "logic_t.h"
#include "qbfequichecker_common.h"
#include <iosfwd>
#include <vector>
#include <string>

namespace QBFEquiChecker_NS
{

class FormulaNodeFactory;

/**
 * @class BinaryOperatorNode
 *
 * Parent class for all binary operator nodes
 */
class BinaryOperatorNode : public OperatorNode
{
    public:
        /**
         * @note The children nodes will not be cloned
         */
        BinaryOperatorNode(FormulaNodeFactory& formulaNodeFactory,
                           FormulaNode& operand1,
                           FormulaNode& operand2,
                           const Index_t& index,
                           const std::string& label);
        
        /**
         * @note The children nodes will not be deleted
         */
		virtual ~BinaryOperatorNode();
	
	
        virtual BinaryOperatorNode *clone() const = 0;


    protected:
		void swap(BinaryOperatorNode& rhs);


        /**
         * @note copy constructor will not clone the child nodes
         */
        BinaryOperatorNode(const BinaryOperatorNode& rhs);
        //BinaryOperatorNode& operator=(const BinaryOperatorNode& rhs);

    private:
        virtual Logic_t evaluate_() = 0;
        virtual void print_(std::ostream& outStrm) const = 0;
        virtual std::string findLabel_(const Index_t& index) const;
        //virtual const Index_t& findIndex_(const std::string& label) const;
        virtual Index_t findVarIndex_(const std::string& varName) const;
        virtual Size_Type getTreeHeight_() const;
        virtual bool setVariableVal_(const Index_t& index, const Logic_t& value);
        virtual FormulaNode *getChild_(unsigned int childIndex);
        virtual const FormulaNode *getChild_(unsigned int childIndex) const;
        virtual bool setChild_(FormulaNode& childNode, unsigned int index);
        virtual Size_Type getNumOfChildren_() const;
        virtual FormulaNode& simplify_() = 0;
        virtual void removeAllChildrenNode_();
        virtual bool removeChildNode_(unsigned int childIndex);
        virtual void deleteTree_();
        virtual FormulaNode& convertCNF_(FormulaNode& cnfNode) = 0;
        virtual FormulaNode& convertISCAS_(std::string& outStr) = 0;

        FormulaNode *operand1_;
        FormulaNode *operand2_;
};


} // namespace QBFEquiChecker_NS


#endif // BINARYOPERATORNODE_H

