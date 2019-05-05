/**
 * @file cnfnode.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#include "cnfnode.h"
#include "formulanodefactory.h"
#include <ostream>


namespace QBFEquiChecker_NS
{

CNFNode::CNFNode(FormulaNodeFactory& formulaNodeFactory,
                 const Index_t& index,
                 const std::string& label) :
        FormulaNode(formulaNodeFactory, index, label)
{
}


CNFNode::~CNFNode()
{
}


CNFNode::CNFNode(const CNFNode& rhs) : 
    FormulaNode(rhs),
    childrenNodes_(rhs.childrenNodes_)
{
}


CNFNode& CNFNode::operator=(const CNFNode& rhs)
{
    CNFNode tmp(rhs);
    this->swap(tmp);
    return *this;
}


CNFNode *CNFNode::clone() const
{
    return new CNFNode(*this);
}


void CNFNode::swap(CNFNode& rhs)
{
    using std::swap;

    swap(childrenNodes_, rhs.childrenNodes_);
    FormulaNode::swap(rhs);
}


FormulaNode *CNFNode::getChild_(unsigned int childIndex)
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

      
const FormulaNode *CNFNode::getChild_(unsigned int childIndex) const
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

 
bool CNFNode::setChild_(FormulaNode& childNode, unsigned int childIndex)
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


std::string CNFNode::findLabel_(const Index_t& index) const
{
    for(unsigned int i = 0; i < childrenNodes_.size(); ++i)
    {
        std::string tmp = childrenNodes_[i]->findLabel(index);
           
        if(tmp.length() > 0)
            return tmp;
    }
    
    return ""; 
}


Index_t CNFNode::findVarIndex_(const std::string& varName) const
{
    for(unsigned int i = 0; i < childrenNodes_.size(); ++i)
    {
        Index_t tmp = childrenNodes_[i]->findVarIndex(varName);

        if(tmp > 0)
            return tmp;
    }

    return 0;
}


FormulaNode& CNFNode::convertCNF_(FormulaNode&)
{
    //assert(0);
    return *this;
}


FormulaNode& CNFNode::convertISCAS_(std::string&)
{
    assert(0);
    return *this;
}



Size_Type CNFNode::getTreeHeight_() const
{
    Size_Type tmp = 0;

    for(unsigned int i = 0; i < childrenNodes_.size(); ++i)
    {
        Size_Type tmp2 = childrenNodes_[i]->getTreeHeight();
        tmp = (tmp > tmp2 ? tmp : tmp2);
    }

    if(tmp < std::numeric_limits<Size_Type>::max())
        return tmp + 1;
    else
        throw FormulaParserException("The tree height is too large!");
}


Size_Type CNFNode::getNumOfChildren_() const
{
    return childrenNodes_.size();
}


Logic_t CNFNode::evaluate_()
{
    Logic_t result(true);

    for(unsigned int i = 0; i < childrenNodes_.size(); ++i)
    {
        result &= (childrenNodes_[i]->evaluate());
    }

    return result;
}


void CNFNode::print_(std::ostream& outStrm) const
{
    outStrm << "(";

    if(!childrenNodes_.empty())
    {
        outStrm << *childrenNodes_[0];

        for(unsigned int i = 1; i < childrenNodes_.size(); ++i)
        {
            outStrm << " & " << *childrenNodes_[i];
        }
    }
 
    outStrm << ")";
}


Node_Type CNFNode::getNodeType_() const
{
    return CNF_NODE;
}


bool CNFNode::setVariableVal_(const Index_t& index, const Logic_t& value)
{
    bool result = false;

    for(unsigned int i = 0; i < childrenNodes_.size(); ++i)
    {
        result |= (childrenNodes_[i]->setVariableValue(index, value));
    }
    
    return result;
}


bool CNFNode::addChildNode_(FormulaNode& childNode)
{
    childrenNodes_.push_back(&childNode);
    return true;
}


FormulaNode& CNFNode::simplify_()
{
    return *this;
}


void CNFNode::removeAllChildrenNode_()
{
    for(unsigned int i = 0; i < childrenNodes_.size(); ++i)
    {
        this->getNodeFactory().removeFormulaNode(*(childrenNodes_[i]));
    }
}


bool CNFNode::removeChildNode_(unsigned int childIndex)
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


void CNFNode::deleteTree_()
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
    void swap<QBFEquiChecker_NS::CNFNode>(
              QBFEquiChecker_NS::CNFNode& lhs,
              QBFEquiChecker_NS::CNFNode& rhs)
    {
        lhs.swap(rhs);
    }
}

