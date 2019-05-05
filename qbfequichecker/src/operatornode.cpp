/**
 * @file operatornode.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#include "operatornode.h"
#include "formulanodefactory.h"
#include <ostream>

namespace QBFEquiChecker_NS
{

OperatorNode::OperatorNode(FormulaNodeFactory& formulaNodeFactory,
                           const Index_t& index,
                           const std::string& label) :
    FormulaNode(formulaNodeFactory, index, label)
{
}


OperatorNode::~OperatorNode()
{
}


OperatorNode::OperatorNode(const OperatorNode& rhs) : FormulaNode(rhs)
{
}


//OperatorNode& OperatorNode::operator=(const OperatorNode& rhs)
//{
//	Node::operator=(rhs);
//    return *this;
//}


void OperatorNode::swap(OperatorNode& rhs)
{
	using std::swap;
	
	FormulaNode::swap(rhs);
}


bool OperatorNode::addChildNode_(FormulaNode&)
{
    return false;
}


} // namespace QBFEquiChecker_NS

