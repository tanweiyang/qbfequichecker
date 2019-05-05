/**
 * @file clausenode.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */

#include "clausenode.h"
#include "formulanodefactory.h"
#include <ostream>
#include <algorithm>


namespace QBFEquiChecker_NS
{

ClauseNode::ClauseNode(FormulaNodeFactory& formulaNodeFactory,
                       const Index_t& index,
                       const std::string& label) :
        FormulaNode(formulaNodeFactory, index, label)
{
}


ClauseNode::~ClauseNode()
{
}


ClauseNode::ClauseNode(const ClauseNode& rhs) : 
    FormulaNode(rhs),
    childrenNodes_(rhs.childrenNodes_)
{
}


ClauseNode& ClauseNode::operator=(const ClauseNode& rhs)
{
    ClauseNode tmp(rhs);
    this->swap(tmp);
    return *this;
}


ClauseNode *ClauseNode::clone() const
{
    return new ClauseNode(*this);
}


void ClauseNode::swap(ClauseNode& rhs)
{
    using std::swap;

    swap(childrenNodes_, rhs.childrenNodes_);
    FormulaNode::swap(rhs);
}


FormulaNode *ClauseNode::getChild_(unsigned int childIndex)
{
    if(childIndex < childrenNodes_.size())
    {
        return childrenNodes_[childIndex];
    }
    else
    {
        return NULL;
    }
}

      
const FormulaNode *ClauseNode::getChild_(unsigned int childIndex) const
{
    if(childIndex < childrenNodes_.size())
    {
        return childrenNodes_[childIndex];
    }
    else
    {
        return NULL;
    }
}

 
bool ClauseNode::setChild_(FormulaNode& childNode, unsigned int childIndex)
{
    if(childIndex < childrenNodes_.size())
    {
        childrenNodes_[childIndex] = &childNode;
        return true;
    }
    else
    {
        return false;
    }
}


FormulaNode& ClauseNode::convertCNF_(FormulaNode&)
{
    assert(0);
    return *this;
}


FormulaNode& ClauseNode::convertISCAS_(std::string&)
{
    assert(0);
    return *this;
}


Size_Type ClauseNode::getTreeHeight_() const
{
    // should be 2 only, since by right all the children nodes should just be literal nodes
    return 2;
}


Size_Type ClauseNode::getNumOfChildren_() const
{
    return childrenNodes_.size();
}


Logic_t ClauseNode::evaluate_()
{
    Logic_t result(false);

    for(unsigned int i = 0; i < childrenNodes_.size(); ++i)
    {
        result |= (childrenNodes_[i]->evaluate());
    }

    return result;
}


std::string ClauseNode::findLabel_(const Index_t& index) const
{
    for(unsigned int i = 0; i < childrenNodes_.size(); ++i)
    {
        std::string tmp = childrenNodes_[i]->findLabel(index);
           
        if(tmp.length() > 0)
            return tmp;
    }
    
    return ""; 
}


Index_t ClauseNode::findVarIndex_(const std::string& varName) const
{
    for(unsigned int i = 0; i < childrenNodes_.size(); ++i)
    {
        Index_t tmp = childrenNodes_[i]->findVarIndex(varName);

        if(tmp > 0)
            return tmp;
    }

    return 0;
}


void ClauseNode::print_(std::ostream& outStrm) const
{
    outStrm << "(";

    if(!childrenNodes_.empty())
    {
        outStrm << *childrenNodes_[0];

        for(unsigned int i = 1; i < childrenNodes_.size(); ++i)
        {
            outStrm << " | " << *childrenNodes_[i];
        }
    }
 
    outStrm << ")";
}


Node_Type ClauseNode::getNodeType_() const
{
    return CLAUSE_NODE;
}


bool ClauseNode::setVariableVal_(const Index_t& index, const Logic_t& value)
{
    bool result = false;

    for(unsigned int i = 0; i < childrenNodes_.size(); ++i)
    {
        result |= (childrenNodes_[i]->setVariableValue(index, value));
    }
    
    return result;
}


bool ClauseNode::addChildNode_(FormulaNode& childNode)
{
    childrenNodes_.push_back(&childNode);
    return true;
}


FormulaNode& ClauseNode::simplify_()
{
    return *this;
}



void ClauseNode::removeAllChildrenNode_()
{
    for(unsigned int i = 0; i < childrenNodes_.size(); ++i)
    {
        this->getNodeFactory().removeFormulaNode(*(childrenNodes_[i]));
    }
}


bool ClauseNode::removeChildNode_(unsigned int childIndex)
{
    if(childIndex < childrenNodes_.size())
    {
        this->getNodeFactory().removeFormulaNode(*(childrenNodes_[childIndex]));
        return true;
    }
    else
    {
        return false;
    }
}


void ClauseNode::deleteTree_()
{
    for(unsigned int i = 0; i < childrenNodes_.size(); ++i)
    {
        childrenNodes_[i]->deleteTree();
    }
    
    //this->getNodeFactory().removeFormulaNodeRecur(*this);
    this->getNodeFactory().removeFormulaNode(*this);
}


} // namespace QBFEquiChecker_NS


namespace std
{
    template<>
    void swap<QBFEquiChecker_NS::ClauseNode>(
              QBFEquiChecker_NS::ClauseNode& lhs,
              QBFEquiChecker_NS::ClauseNode& rhs)
    {
        lhs.swap(rhs);
    }
}

