/**
 * @file falsenode.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */

#include "falsenode.h"
#include "formulanodefactory.h"
#include <ostream>


namespace QBFEquiChecker_NS
{

FalseNode::FalseNode(FormulaNodeFactory& formulaNodeFactory,
                     const Index_t& index) :
    LiteralNode(formulaNodeFactory, index, "", false)
{
}


FalseNode::~FalseNode()
{
}


FalseNode::FalseNode(const FalseNode& rhs) : 
    LiteralNode(rhs) 
{
}


FalseNode& FalseNode::operator=(const FalseNode& rhs)
{
    FalseNode tmp(rhs);
    this->swap(tmp);
    return *this;
}


void FalseNode::swap(FalseNode& rhs)
{
    using std::swap;

    LiteralNode::swap(rhs);
}


FalseNode *FalseNode::clone() const
{
    return new FalseNode(*this);
}


Logic_t FalseNode::evaluate_()
{
    if(isNegated())
    {
        return Logic_t(true);
    }
    else
    {
        return Logic_t(false);
    }
}


void FalseNode::print_(std::ostream& outStrm) const
{
    if(isNegated())
    { 
        outStrm << "1";
    }
    else
    {
        outStrm << "0";
    }
}


std::string FalseNode::findLabel_(const Index_t&) const
{
    return "";
}


Index_t FalseNode::findVarIndex_(const std::string&) const
{
    return this->getIndex();
}


Node_Type FalseNode::getNodeType_() const
{
    return FALSE_NODE;
}


bool FalseNode::setVariableVal_(const Index_t&, const Logic_t&)
{
    return false;
}


} // namespace QBFEquiChecker_NS


namespace std
{
    template<>
    void swap<QBFEquiChecker_NS::FalseNode>(
              QBFEquiChecker_NS::FalseNode& lhs,
              QBFEquiChecker_NS::FalseNode& rhs)
    {
        lhs.swap(rhs);
    }
}

