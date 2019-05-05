/**
 * @file operatorornode.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#include "operatorornode.h"
#include "formulanodefactory.h"
#include "clausenode.h"
#include "cnfnode.h"
#include <ostream>
#include <cassert>

#include <iostream>


namespace QBFEquiChecker_NS
{

OperatorOrNode::OperatorOrNode(FormulaNodeFactory& formulaNodeFactory,
                               FormulaNode& operand1,
                               FormulaNode& operand2,
                               const Index_t& index,
                               const std::string& label) :
    BinaryOperatorNode(formulaNodeFactory, operand1, operand2, index, label)
{
}


OperatorOrNode::~OperatorOrNode()
{
}


OperatorOrNode::OperatorOrNode(const OperatorOrNode& rhs) :
    BinaryOperatorNode(rhs)
{
}


OperatorOrNode& OperatorOrNode::operator=(const OperatorOrNode& rhs)
{
    OperatorOrNode tmp(rhs);
    this->swap(tmp);

    return *this;
}


OperatorOrNode *OperatorOrNode::clone() const
{
	return new OperatorOrNode(*this);
}


void OperatorOrNode::swap(OperatorOrNode& rhs)
{
	using std::swap;

	BinaryOperatorNode::swap(rhs);
}


Logic_t OperatorOrNode::evaluate_()
{
	return (getChild(0)->evaluate() | getChild(1)->evaluate()); 
}


void OperatorOrNode::print_(std::ostream& outStrm) const
{
	outStrm << "(";
    getChild(0)->print(outStrm);
	outStrm << " | ";
    getChild(1)->print(outStrm);
	outStrm << ")";
}


Node_Type OperatorOrNode::getNodeType_() const
{
    return OP_OR_NODE;
}


FormulaNode& OperatorOrNode::simplify_()
{
    assert(getChild(0) != NULL);
    assert(getChild(1) != NULL);

    FormulaNode& leftchild = getChild(0)->simplify();
    FormulaNode& rightchild = getChild(1)->simplify();

    setChild(leftchild, 0);
    setChild(rightchild, 1);

    if(&leftchild == &rightchild)
    {
        this->getNodeFactory().removeFormulaNode(rightchild);
        //rightchild.deleteTree();
        this->getNodeFactory().removeFormulaNode(*this);
        return leftchild;
    }
    else if(leftchild.getNodeType() == TRUE_NODE)
    { 
        //this->getNodeFactory().removeFormulaNode(rightchild);
        rightchild.deleteTree();
        this->getNodeFactory().removeFormulaNode(*this);
 
        return (this->getNodeFactory().createTrueNode());
    }
    else if(rightchild.getNodeType() == TRUE_NODE)
    {
        this->getNodeFactory().removeFormulaNode(*this);
        //this->getNodeFactory().removeFormulaNode(leftchild);
        leftchild.deleteTree();

        return (this->getNodeFactory().createTrueNode());
    }
    else if(leftchild.getNodeType() == FALSE_NODE)
    {
        this->getNodeFactory().removeFormulaNode(*this);
        return rightchild;
    }
    else if(rightchild.getNodeType() == FALSE_NODE)
    {
        this->getNodeFactory().removeFormulaNode(*this);
        return leftchild;
    }
    else if(leftchild.getNodeType() == OP_NOT_NODE)
    {
        if(rightchild.getNodeType() == OP_NOT_NODE &&
           leftchild.getChild(0) == rightchild.getChild(0))
        {
            //this->getNodeFactory().removeFormulaNode(rightchild);
            rightchild.deleteTree();
            this->getNodeFactory().removeFormulaNode(*this);
            
            return leftchild;
        }
        else if(leftchild.getChild(0) == &rightchild)
        {
            //this->getNodeFactory().removeFormulaNode(*this);
            this->deleteTree();
            return (this->getNodeFactory().createTrueNode());
        }
    }
    else if(rightchild.getNodeType() == OP_NOT_NODE)
    {
        if(rightchild.getChild(0) == &leftchild)
        {
            //this->getNodeFactory().removeFormulaNode(*this);
            this->deleteTree();
            return (this->getNodeFactory().createTrueNode());
        }
    }

    return *this;
}


FormulaNode& OperatorOrNode::convertCNF_(FormulaNode& cnfNode)
{
    // a | b = c:
    // 1) a | b | ~c
    // 2) ~a | c
    // 3) ~b | c

    FormulaNode& a = this->getChild(0)->convertCNF(cnfNode);
    FormulaNode& b = this->getChild(1)->convertCNF(cnfNode);

    FormulaNode& c = (this->getNodeFactory().createVariableNode());

    // cannot simplify, because "a" node needs the "not" node!
    FormulaNode *n_a_ptr = NULL;
    if(a.getNodeType() == OP_NOT_NODE)
    {
        n_a_ptr = a.getChild(0);
    }
    else
    {
        n_a_ptr = &(this->getNodeFactory().createOperatorNotNode(a));
    }
    FormulaNode *n_b_ptr = NULL;
    if(b.getNodeType() == OP_NOT_NODE)
    {
        n_b_ptr = b.getChild(0);
    }
    else
    {
        n_b_ptr = &(this->getNodeFactory().createOperatorNotNode(b));
    }

    FormulaNode& n_c = (this->getNodeFactory().createOperatorNotNode(c));


    std::vector<FormulaNode *>clause1;
    clause1.push_back(&a);
    clause1.push_back(&b);
    clause1.push_back(&n_c);

    std::vector<FormulaNode *>clause2;
    clause2.push_back(n_a_ptr);
    clause2.push_back(&c);

    std::vector<FormulaNode *>clause3;
    clause3.push_back(n_b_ptr);
    clause3.push_back(&c);

    cnfNode.addChildNode(this->getNodeFactory().createClauseNode(clause1));
    cnfNode.addChildNode(this->getNodeFactory().createClauseNode(clause2));
    cnfNode.addChildNode(this->getNodeFactory().createClauseNode(clause3));

    return c;
}


FormulaNode& OperatorOrNode::convertISCAS_(std::string& outStr)
{
    // a | b = c:
    // 1) a | b | ~c
    // 2) ~a | c
    // 3) ~b | c

    FormulaNode& a = this->getChild(0)->convertISCAS(outStr);
    FormulaNode& b = this->getChild(1)->convertISCAS(outStr);

    FormulaNode& c = this->getNodeFactory().createVariableNode();

    std::string new_gate = c.getLabel() + " = OR( " + a.getLabel() + ", " + b.getLabel() + ")\n";
    outStr += new_gate;

    return c;
}


} // namespace QBFEquiChecker_NS


namespace std
{
    template<>
    void swap<QBFEquiChecker_NS::OperatorOrNode>(
              QBFEquiChecker_NS::OperatorOrNode& lhs,
              QBFEquiChecker_NS::OperatorOrNode& rhs)
    {
        lhs.swap(rhs);
    }
}

