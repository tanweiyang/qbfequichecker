/**
 * @file truenode.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */



#include "truenode.h"
#include <ostream>


namespace QBFEquiChecker_NS
{

TrueNode::TrueNode(FormulaNodeFactory& formulaNodeFactory,
                   const Index_t& index) :
    LiteralNode(formulaNodeFactory, index, "", false)
{
}


TrueNode::~TrueNode()
{
}


TrueNode::TrueNode(const TrueNode& rhs) : 
    LiteralNode(rhs) 
{
}


TrueNode& TrueNode::operator=(const TrueNode& rhs)
{
    TrueNode tmp(rhs);
    this->swap(tmp);
    return *this;
}


void TrueNode::swap(TrueNode& rhs)
{
    using std::swap;

    LiteralNode::swap(rhs);
}


TrueNode *TrueNode::clone() const
{
    return new TrueNode(*this);
}


Logic_t TrueNode::evaluate_()
{
    if(isNegated())
    {
        return Logic_t(false);
    }
    else
    {
        return Logic_t(true);
    }
}


void TrueNode::print_(std::ostream& outStrm) const
{
    if(isNegated())
    { 
        outStrm << "0";
    }
    else
    {
        outStrm << "1";
    }
}


std::string TrueNode::findLabel_(const Index_t&) const
{
    return "";
}


Index_t TrueNode::findVarIndex_(const std::string&) const
{
    return this->getIndex();
}


Node_Type TrueNode::getNodeType_() const
{
    return TRUE_NODE;
}


bool TrueNode::setVariableVal_(const Index_t&, const Logic_t&)
{
    return false;
}


} // namespace QBFEquiChecker_NS


namespace std
{
    template<>
    void swap<QBFEquiChecker_NS::TrueNode>(
              QBFEquiChecker_NS::TrueNode& lhs,
              QBFEquiChecker_NS::TrueNode& rhs)
    {
        lhs.swap(rhs);
    }
}

