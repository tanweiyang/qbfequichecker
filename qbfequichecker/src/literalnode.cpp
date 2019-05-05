/**
 * @file literalnode.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#include "literalnode.h"
#include "formulanodefactory.h"
#include <ostream>
#include <cassert>


namespace QBFEquiChecker_NS
{

LiteralNode::LiteralNode(FormulaNodeFactory& formulaNodeFactory,
                         const Index_t& index,
                         const std::string& label,
                         bool isNegated) :
    FormulaNode(formulaNodeFactory, index, label),
    isNegated_(isNegated)
{
}


LiteralNode::~LiteralNode()
{
}


LiteralNode::LiteralNode(const LiteralNode& rhs) : 
    FormulaNode(rhs), 
    isNegated_(rhs.isNegated_)
{
}


void LiteralNode::swap(LiteralNode& rhs)
{
    using std::swap;

    swap(isNegated_, rhs.isNegated_);
    FormulaNode::swap(rhs);
}


FormulaNode *LiteralNode::getChild_(unsigned int)
{
    return NULL;
}

      
const FormulaNode *LiteralNode::getChild_(unsigned int) const
{
    return NULL;
}


Size_Type LiteralNode::getTreeHeight_() const
{
    return 1;
}


Size_Type LiteralNode::getNumOfChildren_() const
{
    return 0;
}


bool LiteralNode::addChildNode_(FormulaNode&)
{
    return false;
}


bool LiteralNode::setChild_(FormulaNode&, unsigned int)
{
    return false;
}


FormulaNode& LiteralNode::simplify_()
{
    return *this;
}


void LiteralNode::removeAllChildrenNode_()
{
}


bool LiteralNode::removeChildNode_(unsigned int)
{
    return false;
}


void LiteralNode::deleteTree_()
{
    this->getNodeFactory().removeFormulaNode(*this);
}


FormulaNode& LiteralNode::convertCNF_(FormulaNode& /*cnfNode*/)
{
    return *this;
}


FormulaNode& LiteralNode::convertISCAS_(std::string&)
{
    //outStr = "INPUT( " + this->getLabel() + ")\n" + outStr;

    return *this;
}


} // namespace QBFEquiChecker_NS

