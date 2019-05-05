/**
 * @file formulanode.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#include "formulanode.h"
#include "formulanodefactory.h"
#include <ostream>


namespace QBFEquiChecker_NS
{

FormulaNode::FormulaNode(FormulaNodeFactory& formulaNodeFactory,
                         const Index_t& index,
                         const std::string& label) :
        formulaNodeFactory_(&formulaNodeFactory),
        index_(index),
        label_(label)
{
}


FormulaNode::~FormulaNode()
{
}


FormulaNode::FormulaNode(const FormulaNode& rhs) :
        formulaNodeFactory_(rhs.formulaNodeFactory_),
        index_(rhs.index_),
        label_(rhs.label_)
{
}
 

FormulaNode& FormulaNode::operator=(const FormulaNode& rhs)
{
    formulaNodeFactory_ = rhs.formulaNodeFactory_;
    index_ = rhs.index_;
    label_ = rhs.label_;
    return *this;
}


void FormulaNode::swap(FormulaNode& rhs)
{
    using std::swap;
    swap(formulaNodeFactory_, rhs.formulaNodeFactory_);
    swap(index_, rhs.index_);
    swap(label_, rhs.label_);

}


bool FormulaNode::setVariableValue(Index_t index, const Logic_t& val)
{
    return setVariableVal_(index, val);
}


bool FormulaNode::setVariableValue(Index_t index,
                                   const Logic_t::LOGIC_TYPE_E_& val)
{
    return setVariableVal_(index, Logic_t(val));
}


bool FormulaNode::setVariableValue(Index_t index, bool val)
{
    return setVariableVal_(index, Logic_t(val));
}


void FormulaNode::setIndex(const Index_t& index)
{
    index_ = index;
}


} // namespace QBFEquiChecker_NS


std::ostream& operator<<(std::ostream& outStrm, 
						 const QBFEquiChecker_NS::FormulaNode& output)
{
    output.print(outStrm);
    return outStrm;
}


