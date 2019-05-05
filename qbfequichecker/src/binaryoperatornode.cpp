/**
 * @file binaryoperatornode.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#include "binaryoperatornode.h"
#include "formulanodefactory.h"
#include <ostream>


namespace QBFEquiChecker_NS
{

BinaryOperatorNode::BinaryOperatorNode(FormulaNodeFactory& formulaNodeFactory,
                                       FormulaNode& operand1,
                                       FormulaNode& operand2,
                                       const Index_t& index,
                                       const std::string& label) :
    OperatorNode(formulaNodeFactory, index, label),
    operand1_(&operand1),
    operand2_(&operand2)
{
}


BinaryOperatorNode::~BinaryOperatorNode()
{
//    delete operand1_;
//    delete operand2_;
}


BinaryOperatorNode::BinaryOperatorNode(const BinaryOperatorNode& rhs) :
    OperatorNode(rhs),
    operand1_(rhs.operand1_),
    operand2_(rhs.operand2_)
{
}


//BinaryOperatorNode& BinaryOperatorNode::operator=(const BinaryOperatorNode& rhs)
//{
//    BinaryOperatorNode tmp(rhs);
//    this->swap(tmp);

//    return *this;
//}


void BinaryOperatorNode::swap(BinaryOperatorNode& rhs)
{
	using std::swap;

	OperatorNode::swap(rhs);
    swap(this->operand1_, rhs.operand1_);
    swap(this->operand2_, rhs.operand2_);
}

     
FormulaNode *BinaryOperatorNode::getChild_(unsigned int childIndex)
{
    switch(childIndex)
    {
        case 0: return operand1_;
        case 1: return operand2_;
        default: return NULL;
    }
}       


const FormulaNode *BinaryOperatorNode::getChild_(unsigned int childIndex) const
{
    switch(childIndex)
    {
        case 0: return operand1_;
        case 1: return operand2_;
        default: return NULL;
    }
}       


bool BinaryOperatorNode::setChild_(FormulaNode& childNode, unsigned int childIndex)
{
    switch(childIndex)
    {
        case 0: 
            operand1_ = &childNode;
            return true;

        case 1:
            operand2_ = &childNode;
            return true;

        default: 
            return false;
    }
}       


Size_Type BinaryOperatorNode::getNumOfChildren_() const
{
    return 2;
}


std::string BinaryOperatorNode::findLabel_(const Index_t& index) const
{
    std::string tmp = operand1_->findLabel(index);
    if(tmp.length() > 0)
        return tmp;
    else
        return operand2_->findLabel(index);
}


Index_t BinaryOperatorNode::findVarIndex_(const std::string& varName) const
{
    Index_t tmp = operand1_->findVarIndex(varName);
    if(tmp > 0)
        return tmp;
    else
        return operand2_->findVarIndex(varName);
}


Size_Type BinaryOperatorNode::getTreeHeight_() const
{
    Size_Type height1 = operand1_->getTreeHeight();
    Size_Type height2 = operand2_->getTreeHeight();

    Size_Type tmp = (height1 > height2 ? height1 : height2);

    increment(tmp);

    return tmp;

//    if(tmp < std::numeric_limits<Size_Type>::max())
//        return tmp + 1;
//    else
//        throw FormulaParserException("The tree height is too large!");
}


bool BinaryOperatorNode::setVariableVal_(const Index_t& index,
                                         const Logic_t& value)
{
    bool o1 = operand1_->setVariableValue(index, value);
    bool o2 = operand2_->setVariableValue(index, value);

    return o1 || o2;
}


void BinaryOperatorNode::removeAllChildrenNode_()
{
    this->getNodeFactory().removeFormulaNode(*operand1_);
    this->getNodeFactory().removeFormulaNode(*operand2_);
}


bool BinaryOperatorNode::removeChildNode_(unsigned int childIndex)
{
    switch(childIndex)
    {
        case 0:
            this->getNodeFactory().removeFormulaNode(*operand1_);
            return true;

        case 1:
            this->getNodeFactory().removeFormulaNode(*operand2_);
            return true;

        default:
            return false;
    }
}


void BinaryOperatorNode::deleteTree_()
{
    operand1_->deleteTree();
    operand2_->deleteTree();
    
    operand1_ = NULL;
    operand2_ = NULL;

    this->getNodeFactory().removeFormulaNode(*this);
}


} // namespace QBFEquiChecker_NS

