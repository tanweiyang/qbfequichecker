/**
 * @file operatorandnode.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#include "operatorandnode.h"
#include "formulanodefactory.h"
#include <ostream>
#include <cassert>


namespace QBFEquiChecker_NS
{

OperatorAndNode::OperatorAndNode(FormulaNodeFactory& formulaNodeFactory,
                                 FormulaNode& operand1,
                                 FormulaNode& operand2,
                                 const Index_t& index,
                                 const std::string& label) :
    BinaryOperatorNode(formulaNodeFactory, operand1, operand2, index, label)
{
}


OperatorAndNode::~OperatorAndNode()
{
}


OperatorAndNode::OperatorAndNode(const OperatorAndNode& rhs) :
    BinaryOperatorNode(rhs)
{
}


OperatorAndNode& OperatorAndNode::operator=(const OperatorAndNode& rhs)
{
    OperatorAndNode tmp(rhs);
    this->swap(tmp);

    return *this;
}


OperatorAndNode *OperatorAndNode::clone() const
{
	return new OperatorAndNode(*this);
}


void OperatorAndNode::swap(OperatorAndNode& rhs)
{
	using std::swap;

	BinaryOperatorNode::swap(rhs);
}


Logic_t OperatorAndNode::evaluate_()
{
	return (this->getChild(0)->evaluate() & this->getChild(1)->evaluate()); 
}


void OperatorAndNode::print_(std::ostream& outStrm) const
{
	outStrm << "(";
    this->getChild(0)->print(outStrm);
	outStrm << " & ";
    this->getChild(1)->print(outStrm);
	outStrm << ")";
}


Node_Type OperatorAndNode::getNodeType_() const
{
    return OP_AND_NODE;
}


FormulaNode& OperatorAndNode::convertCNF_(FormulaNode& cnfNode)
{
    // a & b = c:
    // 1) ~a | ~b | c
    // 2) a | ~c
    // 3) b | ~c

    FormulaNode& a = this->getChild(0)->convertCNF(cnfNode);
    FormulaNode& b = this->getChild(1)->convertCNF(cnfNode);

    FormulaNode& c = this->getNodeFactory().createVariableNode();

    // cannot simplify!
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

    FormulaNode& n_c = this->getNodeFactory().createOperatorNotNode(c);

    std::vector<FormulaNode *>clause1;
    clause1.push_back(n_a_ptr);
    clause1.push_back(n_b_ptr);
    clause1.push_back(&c);

    std::vector<FormulaNode *>clause2;
    clause2.push_back(&a);
    clause2.push_back(&n_c);

    std::vector<FormulaNode *>clause3;
    clause3.push_back(&b);
    clause3.push_back(&n_c);

    cnfNode.addChildNode(this->getNodeFactory().createClauseNode(clause1));
    cnfNode.addChildNode(this->getNodeFactory().createClauseNode(clause2));
    cnfNode.addChildNode(this->getNodeFactory().createClauseNode(clause3));

    return c;
}


FormulaNode& OperatorAndNode::convertISCAS_(std::string& outStr)
{
    // a & b = c:
    // 1) ~a | ~b | c
    // 2) a | ~c
    // 3) b | ~c

    FormulaNode& a = this->getChild(0)->convertISCAS(outStr);
    FormulaNode& b = this->getChild(1)->convertISCAS(outStr);

    FormulaNode& c = this->getNodeFactory().createVariableNode();

    std::string new_gate = c.getLabel() + " = AND( " + a.getLabel() + ", " + b.getLabel() + ")\n";
    outStr += new_gate;

    return c;
}


FormulaNode& OperatorAndNode::simplify_()
{
    assert(getChild(0) != NULL);
    assert(getChild(1) != NULL);

    FormulaNode& leftchild = getChild(0)->simplify();
    FormulaNode& rightchild = getChild(1)->simplify();
    
    //leftchild = leftchild.simplify();
    //rightchild = rightchild.simplify();

    setChild(leftchild, 0);
    setChild(rightchild, 1);

    if(&leftchild == &rightchild)
    {
        this->removeChildNode(1);
        //rightchild.deleteTree();
        this->getNodeFactory().removeFormulaNode(*this);
 
        return leftchild;
    }
    else if(leftchild.getNodeType() == FALSE_NODE)
    {
        //this->removeChildNode(1);
        rightchild.deleteTree();
        this->getNodeFactory().removeFormulaNode(*this);
 
        return this->getNodeFactory().createFalseNode();
    }
    else if(rightchild.getNodeType() == FALSE_NODE)
    {
        //this->removeChildNode(0);
        leftchild.deleteTree();
        this->getNodeFactory().removeFormulaNode(*this);
 
        return this->getNodeFactory().createFalseNode();
    }
    else if(leftchild.getNodeType() == TRUE_NODE)
    {
        this->getNodeFactory().removeFormulaNode(*this);
        return rightchild;
    }
    else if(rightchild.getNodeType() == TRUE_NODE)
    {
        this->getNodeFactory().removeFormulaNode(*this);
        return leftchild;
    }
    else if(leftchild.getNodeType() == OP_NOT_NODE)
    {
        // two NOTs
        if(rightchild.getNodeType() == OP_NOT_NODE &&
           leftchild.getChild(0) == rightchild.getChild(0))
        {
            //this->removeChildNode(1);
            rightchild.deleteTree();
            this->getNodeFactory().removeFormulaNode(*this);
            return leftchild;
        }
        else if(leftchild.getChild(0) == &rightchild)
        {
            //this->getNodeFactory().removeFormulaNode(*this);
            this->deleteTree();
            return this->getNodeFactory().createFalseNode();
        }
    }
    else if(rightchild.getNodeType() == OP_NOT_NODE)
    {
        if(rightchild.getChild(0) == &leftchild)
        {
            //this->getNodeFactory().removeFormulaNode(*this);
            this->deleteTree();
            return this->getNodeFactory().createFalseNode();
        }
    }

    return *this;
}

} // namespace QBFEquiChecker_NS


namespace std
{
    template<>
    void swap<QBFEquiChecker_NS::OperatorAndNode>(
              QBFEquiChecker_NS::OperatorAndNode& lhs,
              QBFEquiChecker_NS::OperatorAndNode& rhs)
    {
        lhs.swap(rhs);
    }
}

