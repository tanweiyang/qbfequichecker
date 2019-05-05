/**
 * @file operatornotnode.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#include "operatornotnode.h"
#include "operatorandnode.h"
#include "operatorornode.h"
#include "formulanodefactory.h"
#include <ostream>
#include <cassert>


namespace QBFEquiChecker_NS
{

OperatorNotNode::OperatorNotNode(FormulaNodeFactory& formulaNodeFactory,
                                 FormulaNode& operand,
                                 const Index_t& index,
                                 const std::string& label) :
    UnaryOperatorNode(formulaNodeFactory, operand, index, label)
{
}


OperatorNotNode::~OperatorNotNode()
{
}


OperatorNotNode::OperatorNotNode(const OperatorNotNode& rhs) :
    UnaryOperatorNode(rhs)
{
}


OperatorNotNode& OperatorNotNode::operator=(const OperatorNotNode& rhs)
{
    OperatorNotNode tmp(rhs);
    this->swap(tmp);

    return *this;
}


OperatorNotNode *OperatorNotNode::clone() const
{
	return new OperatorNotNode(*this);
}


void OperatorNotNode::swap(OperatorNotNode& rhs)
{
	using std::swap;

	UnaryOperatorNode::swap(rhs);
}


Logic_t OperatorNotNode::evaluate_()
{
	return (~getChild(0)->evaluate()); 
}


void OperatorNotNode::print_(std::ostream& outStrm) const
{
	outStrm << "(~";
    getChild(0)->print(outStrm);
	outStrm << ")";
}


Node_Type OperatorNotNode::getNodeType_() const
{
    return OP_NOT_NODE;
}


FormulaNode& OperatorNotNode::simplify_()
{
    FormulaNode *returningNode = NULL;
    FormulaNode *child = this->getChild(0);
    assert(child != NULL);

    switch(child->getNodeType())
    {
        case OP_NOT_NODE:
            // remove double negation
            this->getNodeFactory().removeFormulaNode(*child);
            this->getNodeFactory().removeFormulaNode(*this);

            return child->getChild(0)->simplify();


        case TRUE_NODE:
            this->getNodeFactory().removeFormulaNode(*this);
            return (this->getNodeFactory().createFalseNode());


        case FALSE_NODE:
            this->getNodeFactory().removeFormulaNode(*this);
            return (this->getNodeFactory().createTrueNode());


        default:
            returningNode = this;
            child = &(child->simplify());
            this->setChild(*child, 0);
            break;
    }
 
    // after executing simplify()
    
    switch(child->getNodeType())
    {
        case OP_NOT_NODE:
            // remove double negation
            this->getNodeFactory().removeFormulaNode(*child);
            this->getNodeFactory().removeFormulaNode(*this);

            return *(child->getChild(0));


       case TRUE_NODE:
            this->getNodeFactory().removeFormulaNode(*this);
            returningNode = &(this->getNodeFactory().createFalseNode());
            break;


        case FALSE_NODE:
            this->getNodeFactory().removeFormulaNode(*this);
            returningNode = &(this->getNodeFactory().createTrueNode());
            break;


        default:
            break;
    }
   
    return *returningNode;
}


FormulaNode& OperatorNotNode::convertCNF_(FormulaNode& cnfNode)
{
    FormulaNode& childNode = this->getChild(0)->convertCNF(cnfNode);
    //this->setChild(this->getChild(0)->convertCNF(cnfNode), 0);

    if(childNode.getNodeType() == OP_NOT_NODE)
    {
        return *(childNode.getChild(0));
    }
    else
    {
        return this->getNodeFactory().createOperatorNotNode(childNode);
    }

    // okay, now the thing is, childNode may be a NOT node.
    //FormulaNode& returning_node =
    //        this->getNodeFactory().createOperatorNotNode(childNode);
    //FormulaNode& returning_node = simplify();

    //assert(returning_node);

    //return returning_node;

    // there is no need to simplify, since
    //return *this;
}


FormulaNode& OperatorNotNode::convertISCAS_(std::string& outStr)
{
    FormulaNode& a = this->getChild(0)->convertISCAS(outStr);
    FormulaNode& b = this->getNodeFactory().createVariableNode();

    std::string new_gate = b.getLabel() + " = NOT( " + a.getLabel() + ")\n";
    outStr += new_gate;

    return b;
}


} // namespace QBFEquiChecker_NS


namespace std
{
    template<>
    void swap<QBFEquiChecker_NS::OperatorNotNode>(
              QBFEquiChecker_NS::OperatorNotNode& lhs,
              QBFEquiChecker_NS::OperatorNotNode& rhs)
    {
        lhs.swap(rhs);
    }
}

