/**
 * @file operatorxornode.cpp
 * @author Wei Yang Tan
 * @date 28 Nov 2012
 */


#include "operatorxornode.h"
#include "formulanodefactory.h"
#include "clausenode.h"
#include "cnfnode.h"
#include <ostream>
#include <cassert>

#include <iostream>


namespace QBFEquiChecker_NS
{

OperatorXorNode::OperatorXorNode(FormulaNodeFactory& formulaNodeFactory,
                                 FormulaNode& operand1,
                                 FormulaNode& operand2,
                                 const Index_t& index,
                                 const std::string& label) :
    BinaryOperatorNode(formulaNodeFactory, operand1, operand2, index, label)
{
}


OperatorXorNode::~OperatorXorNode()
{
}


OperatorXorNode::OperatorXorNode(const OperatorXorNode& rhs) :
    BinaryOperatorNode(rhs)
{
}


OperatorXorNode& OperatorXorNode::operator=(const OperatorXorNode& rhs)
{
    OperatorXorNode tmp(rhs);
    this->swap(tmp);

    return *this;
}


OperatorXorNode *OperatorXorNode::clone() const
{
	return new OperatorXorNode(*this);
}


void OperatorXorNode::swap(OperatorXorNode& rhs)
{
	using std::swap;

	BinaryOperatorNode::swap(rhs);
}


Logic_t OperatorXorNode::evaluate_()
{
	return (getChild(0)->evaluate() ^ getChild(1)->evaluate());
}


void OperatorXorNode::print_(std::ostream& outStrm) const
{
	outStrm << "(";
    getChild(0)->print(outStrm);
	outStrm << " ^ ";
    getChild(1)->print(outStrm);
	outStrm << ")";
}


Node_Type OperatorXorNode::getNodeType_() const
{
    return OP_XOR_NODE;
}


FormulaNode& OperatorXorNode::simplify_()
{
    assert(getChild(0) != NULL);
    assert(getChild(1) != NULL);

    FormulaNode& leftchild = getChild(0)->simplify();
    FormulaNode& rightchild = getChild(1)->simplify();

    setChild(leftchild, 0);
    setChild(rightchild, 1);

    if(&leftchild == &rightchild)
    {
        this->deleteTree();
        return (this->getNodeFactory().createFalseNode());
    }
    else if(leftchild.getNodeType() == OP_NOT_NODE)
    {
        if(rightchild.getNodeType() == OP_NOT_NODE &&
           leftchild.getChild(0) == rightchild.getChild(0))
        {
            this->deleteTree();
            return (this->getNodeFactory().createFalseNode());
        }
        else if(leftchild.getChild(0) == &rightchild)
        {
            this->deleteTree();
            return (this->getNodeFactory().createTrueNode());
        }
    }
    else if(rightchild.getNodeType() == OP_NOT_NODE)
    {
        if(rightchild.getChild(0) == &leftchild)
        {
            this->deleteTree();
            return (this->getNodeFactory().createTrueNode());
        }
    }

    return *this;
}


FormulaNode& OperatorXorNode::convertCNF_(FormulaNode& cnfNode)
{

    // c = a ^ b
    // => (~a | ~b | ~c) & (a | b | ~c) & (a | ~b | c) & (~a | b | c)

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
    clause1.push_back(n_a_ptr);
    clause1.push_back(n_b_ptr);
    clause1.push_back(&n_c);

    std::vector<FormulaNode *>clause2;
    clause2.push_back(&a);
    clause2.push_back(&b);
    clause2.push_back(&n_c);

    std::vector<FormulaNode *>clause3;
    clause3.push_back(&a);
    clause3.push_back(n_b_ptr);
    clause3.push_back(&c);

    std::vector<FormulaNode *>clause4;
    clause4.push_back(n_a_ptr);
    clause4.push_back(&b);
    clause4.push_back(&c);

    cnfNode.addChildNode(this->getNodeFactory().createClauseNode(clause1));
    cnfNode.addChildNode(this->getNodeFactory().createClauseNode(clause2));
    cnfNode.addChildNode(this->getNodeFactory().createClauseNode(clause3));
    cnfNode.addChildNode(this->getNodeFactory().createClauseNode(clause4));

    return c;
}


FormulaNode& OperatorXorNode::convertISCAS_(std::string& outStr)
{

    // c = a ^ b
    // => c = (~a & b) | (a & ~b)

    FormulaNode& a = this->getChild(0)->convertISCAS(outStr);
    FormulaNode& b = this->getChild(1)->convertISCAS(outStr);

    FormulaNode& n_a = this->getNodeFactory().createOperatorNotNode(a);
    FormulaNode& n_b = this->getNodeFactory().createOperatorNotNode(b);

    FormulaNode& n_a_conv = n_a.convertISCAS(outStr);
    FormulaNode& n_b_conv = n_b.convertISCAS(outStr);

    FormulaNode& oper1 = this->getNodeFactory().createOperatorAndNode(n_a, b);
    FormulaNode& oper2 = this->getNodeFactory().createOperatorAndNode(a, n_b);

    FormulaNode& oper1_conv = oper1.convertISCAS(outStr);
    FormulaNode& oper2_conv = oper2.convertISCAS(outStr);

    FormulaNode& c = this->getNodeFactory().createOperatorOrNode(oper1, oper2);

    return c.convertISCAS(outStr);
}


} // namespace QBFEquiChecker_NS


namespace std
{
    template<>
    void swap<QBFEquiChecker_NS::OperatorXorNode>(
              QBFEquiChecker_NS::OperatorXorNode& lhs,
              QBFEquiChecker_NS::OperatorXorNode& rhs)
    {
        lhs.swap(rhs);
    }
}

