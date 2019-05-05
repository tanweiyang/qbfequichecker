/**
 * @file variablenode.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#include "variablenode.h"
#include "formulanodefactory.h"
#include "cnfnode.h"
#include <ostream>
//#include <cassert>


namespace QBFEquiChecker_NS
{

VariableNode::VariableNode(FormulaNodeFactory& formulaNodeFactory,
                           const std::string& label,
                           const Logic_t::LOGIC_TYPE_E_& value,
                           const Index_t& index,
                           bool isNegated) :
    LiteralNode(formulaNodeFactory, index, label, isNegated),
    value_(value)
{
}


VariableNode::VariableNode(FormulaNodeFactory& formulaNodeFactory,
                           const std::string& label,
                           bool value,
                           const Index_t& index,
                           bool isNegated) :
    LiteralNode(formulaNodeFactory, index, label, isNegated),
    value_(value)
{
}


VariableNode::~VariableNode()
{
}


VariableNode::VariableNode(const VariableNode& rhs) : 
    LiteralNode(rhs),
    value_(rhs.value_) 
{
}


VariableNode& VariableNode::operator=(const VariableNode& rhs)
{
    VariableNode tmp(rhs);
    this->swap(tmp);
    return *this;
}


void VariableNode::swap(VariableNode& rhs)
{
    using std::swap;

    swap(value_, rhs.value_);
    LiteralNode::swap(rhs);
}


VariableNode *VariableNode::clone() const
{
    return new VariableNode(*this);
}


Logic_t VariableNode::evaluate_()
{
    if(isNegated())
    {
        return ~value_;
    }
    else
    {
        return value_;
    }
}


std::string VariableNode::findLabel_(const Index_t& index) const
{
    if(index == this->getIndex())
        return this->getLabel();
    else
        return ""; 
}


Index_t VariableNode::findVarIndex_(const std::string& varName) const
{
    if(varName == this->getLabel())
        return this->getIndex();
    else
        return 0;
}


void VariableNode::print_(std::ostream& outStrm) const
{
    if(isNegated())
    {
        outStrm << "~";
    }

    outStrm << this->getLabel();
}


Node_Type VariableNode::getNodeType_() const
{
    return VAR_NODE;
}


bool VariableNode::setVariableVal_(const Index_t& index, const Logic_t& value)
{
    if(index == getIndex())
    {
        value_ = value;
        return true;
    }
    return false;
}


} // namespace QBFEquiChecker_NS


namespace std
{
    template<>
    void swap<QBFEquiChecker_NS::VariableNode>(
              QBFEquiChecker_NS::VariableNode& lhs,
              QBFEquiChecker_NS::VariableNode& rhs)
    {
        lhs.swap(rhs);
    }
}

