/**
 * @file unaryoperatornode.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#include "unaryoperatornode.h"
#include "formulanodefactory.h"
#include <ostream>

namespace QBFEquiChecker_NS
{

UnaryOperatorNode::UnaryOperatorNode(FormulaNodeFactory& formulaNodeFactory,
                                     FormulaNode& operand,
                                     const Index_t& index,
                                     const std::string& label) :
    OperatorNode(formulaNodeFactory, index, label),
    operand_(&operand)
{
}


UnaryOperatorNode::~UnaryOperatorNode()
{
//    delete operand_;
}


UnaryOperatorNode::UnaryOperatorNode(const UnaryOperatorNode& rhs) :
    OperatorNode(rhs),
    operand_(rhs.operand_)
{
}


//UnaryOperatorNode& UnaryOperatorNode::operator=(const UnaryOperatorNode& rhs)
//{
//    UnaryOperatorNode tmp(rhs);
//    this->swap(tmp);

//    return *this;
//}


void UnaryOperatorNode::swap(UnaryOperatorNode& rhs)
{
	using std::swap;

	OperatorNode::swap(rhs);
    swap(this->operand_, rhs.operand_);
}

     
FormulaNode *UnaryOperatorNode::getChild_(unsigned int childIndex)
{
    switch(childIndex)
    {
        case 0: return operand_;
        default: return NULL;
    }
}       


const FormulaNode *UnaryOperatorNode::getChild_(unsigned int childIndex) const
{
    switch(childIndex)
    {
        case 0: return operand_;
        default: return NULL;
    }
}       


bool UnaryOperatorNode::setChild_(FormulaNode& childNode, unsigned int childIndex)
{
    switch(childIndex)
    {
        case 0: 
            operand_ = &childNode;
            return true;

        default: 
            return false;
    }
}


Size_Type UnaryOperatorNode::getTreeHeight_() const
{
    Size_Type tmp = operand_->getTreeHeight();

    increment(tmp);

    return tmp;
}


Size_Type UnaryOperatorNode::getNumOfChildren_() const
{
    return 1;
}


std::string UnaryOperatorNode::findLabel_(const Index_t& index) const
{
    return operand_->findLabel(index);
}


Index_t UnaryOperatorNode::findVarIndex_(const std::string& varName) const
{
    return operand_->findVarIndex(varName);
}


bool UnaryOperatorNode::setVariableVal_(const Index_t& index, const Logic_t& value)
{
    return operand_->setVariableValue(index, value);
}


void UnaryOperatorNode::removeAllChildrenNode_()
{
    this->getNodeFactory().removeFormulaNode(*operand_);
}


bool UnaryOperatorNode::removeChildNode_(unsigned int childIndex)
{
    switch(childIndex)
    {
        case 0:
            this->getNodeFactory().removeFormulaNode(*operand_);
            return true;

        default:
            return false;
    }
}


void UnaryOperatorNode::deleteTree_()
{
    operand_->deleteTree();
    
    operand_ = NULL;

    this->getNodeFactory().removeFormulaNode(*this);
    //this->getNodeFactory().removeFormulaNodeRecur(*this);
}


} // namespace QBFEquiChecker_NS

