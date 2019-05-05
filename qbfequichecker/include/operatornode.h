/**
 * @file operatornode.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */



#ifndef OPERATORNODE_H
#define OPERATORNODE_H

#include "formulanode.h"
#include "qbfequichecker_common.h"
#include <iosfwd>
#include <vector>
#include <string>

namespace QBFEquiChecker_NS
{

class FormulaNodeFactory;

/**
 * The abstract class for all kinds of operator nodes
 */
class OperatorNode : public FormulaNode
{
    public:
        explicit OperatorNode(FormulaNodeFactory& formulaNodeFactory,
                              const Index_t& index,
                              const std::string& label);
        virtual ~OperatorNode() = 0;

		virtual OperatorNode *clone() const = 0;


    protected:		
        OperatorNode(const OperatorNode& rhs);
        OperatorNode& operator=(const OperatorNode& rhs);
        void swap(OperatorNode& rhs);		


    private:
        virtual Logic_t evaluate_() = 0;
        virtual void print_(std::ostream& outStrm) const = 0;
        virtual Node_Type getNodeType_() const = 0;
        virtual std::string findLabel_(const Index_t& index) const = 0;
        virtual Index_t findVarIndex_(const std::string& varName) const = 0;
        virtual Size_Type getTreeHeight_() const = 0;
        virtual bool setVariableVal_(const Index_t& index, const Logic_t& value) = 0;
        virtual FormulaNode *getChild_(unsigned int childIndex) = 0;
        virtual const FormulaNode *getChild_(unsigned int childIndex) const = 0;
        virtual Size_Type getNumOfChildren_() const = 0;
        virtual bool addChildNode_(FormulaNode&);
        virtual bool setChild_(FormulaNode& childNode, unsigned int index) = 0;
        virtual FormulaNode& simplify_() = 0;
        virtual void removeAllChildrenNode_() = 0;
        virtual bool removeChildNode_(unsigned int childIndex) = 0;
        virtual void deleteTree_() = 0;
        virtual FormulaNode& convertCNF_(FormulaNode& cnfNode) = 0;
        virtual FormulaNode& convertISCAS_(std::string& outStr) = 0;
};

} // namespace QBFEquiChecker_NS


#endif // OPERATORNODE_H

