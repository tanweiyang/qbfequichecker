/**
 * @file formulanodefactory.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#include "formulanodefactory.h"
#include "formulanode.h"
#include "binaryoperatornode.h"
#include "unaryoperatornode.h"
#include "operatorornode.h"
#include "operatorxornode.h"
#include "operatornotnode.h"
#include "operatorandnode.h"
#include "literalnode.h"
#include "variablenode.h"
#include "cnfnode.h"
#include "clausenode.h"
#include "truenode.h"
#include "falsenode.h"
#include "operatornode.h"

#include "formulaparserexception.h"
#include "qbfequichecker_common.h"
#include "uncopyable.h"

//#include <memory> // for std::auto_ptr
#include <cstddef>  // for size_t
#include <cassert>
#include <limits>
#include <vector>
#include <algorithm>
#include <sstream>


#include <iostream>
template<typename T>
void printVector(const std::vector<T>& x)
{
    for(int i = 0; i < x.size(); ++i)
        std::cerr << x[i] << " ";
}
template<typename T>
void printVector2(const std::vector<T>& x)
{
    for(int i = 0; i < x.size(); ++i)
        std::cerr << *(x[i]) << " ";
}



namespace QBFEquiChecker_NS
{

/**
 * Process all the nodes
 */
template<typename T1, typename T2>
void processNodesPtrs(T1& nodePtrs, T2& nodeCnts);


// ###############################################
// PImpl DEFINITIONS
// ###############################################
// compare labels that are are not empty strings and are equal
struct CmpLabel
{
    public:
        explicit CmpLabel(const std::string& label) : label_(label){}

        bool operator()(FormulaNode *lhs)
        {
            if(lhs->getLabel().length() > 0 &&
               lhs->getLabel() == label_)
                return true;

            return false;
        }

    private:
        std::string label_;

};


struct CmpNodeType
{
    public:
        explicit CmpNodeType(Node_Type node_type) : node_type_(node_type){}

        template <typename T>
        bool operator()(T& lhs)
        {
            if(lhs.getNodeType() == node_type_)
                return true;

            return false;
        }


    private:
        Node_Type node_type_; 
};


struct FormulaNodeFactory::PImpl : private Uncopyable
{

    struct DeleteNodeFunctor
    {
        template <typename T>
        void operator()(T *ptr)
        {
            delete ptr;
        }
    };


    PImpl(FormulaNodeFactory&);
    ~PImpl();


    /**
     * Generate a new index label
     */
    std::string generateNewLabel();
 

    /*
     * Free all the nodes heap memory
     */
    void deleteNodes();

    std::vector<VariableNode *> varNodesVector_;
    std::vector<FormulaNode *> opNodesVector_;
    FormulaNode *truefalseNodePtr_[2];
        
    std::vector<Size_Type> varNodesCnt_;
    std::vector<Size_Type> opNodesCnt_;

    Index_t intermediateVarCnt_;
    FormulaNodeFactory& formulaNodeFactory_;
};


// ###############################################
// CLASS FUNCTION DEFINITIONS
// ###############################################
FormulaNodeFactory::FormulaNodeFactory()
{
    pImpl_ = new PImpl(*this);
}


FormulaNodeFactory::~FormulaNodeFactory()
{
    delete pImpl_;
}


FormulaNode& FormulaNodeFactory::createVariableNode(const std::string& label,
                                                    const Logic_t::LOGIC_TYPE_E_& value,
                                                    bool isNegated)
{
    // find if same label variable exists
    std::vector<VariableNode *>::iterator iter = std::find_if(
         pImpl_->varNodesVector_.begin(),
         pImpl_->varNodesVector_.end(),
         CmpLabel(label));
    

    // does not exist; so create a new VariableNode
    if(pImpl_->varNodesVector_.end() == iter)
    {
        return createLiteralNode(label, value, isNegated);
    }
    else
    {
        ++(pImpl_->varNodesCnt_[(*iter)->getIndex() - 1]);
        return *(*iter);
    }
}
 

FormulaNode& FormulaNodeFactory::createLiteralNode(const std::string& label,
                                                   const Logic_t::LOGIC_TYPE_E_& value,
                                                   bool isNegated)
{
    int counter = pImpl_->varNodesVector_.size();
    increment(counter);
    std::string varLabel = label;   
     
    if(varLabel.length() == 0)
    {
        varLabel = pImpl_->generateNewLabel();
    }    

    VariableNode *varNode = new VariableNode(*this, varLabel, value, counter, isNegated);
    pImpl_->varNodesVector_.push_back(varNode);
    pImpl_->varNodesCnt_.push_back(1);
 
    return *varNode;
}


FormulaNode& FormulaNodeFactory::createVariableNode(const std::string& label,
                                                    bool value,
                                                    bool isNegated)
{
    Logic_t::LOGIC_TYPE_E_ l_value = (value == true ? Logic_t::TRUE : Logic_t::FALSE);

    return createVariableNode(label, l_value, isNegated);
}


FormulaNode& FormulaNodeFactory::createLiteralNode(const std::string& label,
                                                   bool value,
                                                   bool isNegated)
{
    Logic_t::LOGIC_TYPE_E_ l_value = (value == true ? Logic_t::TRUE : Logic_t::FALSE);

    return createLiteralNode(label, l_value, isNegated);
}


FormulaNode& FormulaNodeFactory::createTrueNode()
{
    return *(pImpl_->truefalseNodePtr_[TRUE_NODE]);
}
 

FormulaNode& FormulaNodeFactory::createFalseNode()
{
    return *(pImpl_->truefalseNodePtr_[FALSE_NODE]);
}


FormulaNode& FormulaNodeFactory::createOperatorAndNode(FormulaNode& operand1,
                                                       FormulaNode& operand2,
                                                       const std::string& label)
{
    int counter = pImpl_->opNodesVector_.size();
    increment(counter);
    
    OperatorNode *opNode = new OperatorAndNode(*this, operand1, operand2, counter, label);
    
    if(opNode)
    {
        pImpl_->opNodesVector_.push_back(opNode);     
        pImpl_->opNodesCnt_.push_back(1);
    }
    else
    {
        throw FormulaParserException("Failed in creating OperatorAndNode!");
    }

    return *opNode;
}
 

FormulaNode& FormulaNodeFactory::createOperatorOrNode(FormulaNode& operand1,
                                                      FormulaNode& operand2,
                                                      const std::string& label)
{
    int counter = pImpl_->opNodesVector_.size();
    increment(counter);
    
    OperatorNode *opNode = new OperatorOrNode(*this, operand1, operand2, counter, label);
    
    if(opNode)
    {
        pImpl_->opNodesVector_.push_back(opNode);     
        pImpl_->opNodesCnt_.push_back(1);
    }
    else
    {
        throw FormulaParserException("Failed in creating OperatorOrNode!");
    }

    return *opNode;
}
 

FormulaNode& FormulaNodeFactory::createOperatorXorNode(FormulaNode& operand1,
                                                       FormulaNode& operand2,
                                                       const std::string& label)
{
    int counter = pImpl_->opNodesVector_.size();
    increment(counter);

    OperatorNode *opNode = new OperatorXorNode(*this, operand1, operand2, counter, label);

    if(opNode)
    {
        pImpl_->opNodesVector_.push_back(opNode);
        pImpl_->opNodesCnt_.push_back(1);
    }
    else
    {
        throw FormulaParserException("Failed in creating OperatorXorNode!");
    }

    return *opNode;
}


FormulaNode& FormulaNodeFactory::createOperatorNotNode(FormulaNode& operand,
                                                       const std::string& label)
{
    /*
    std::string tmp_label = label;

    // the point here is I don't want to duplicate another NOT node which
    // I already have a NOT node attached to a variable node
    if(label.length() == 0)
    {
        if(operand.getNodeType() == VAR_NODE)
        {
            tmp_label = "~" + operand.getLabel();
        }
    }

    // find if same label variable exists
    std::vector<FormulaNode *>::iterator iter = std::find_if(
         pImpl_->opNodesVector_.begin(),
         pImpl_->opNodesVector_.end(),
         CmpLabel(tmp_label));
    
    // does not exist; so create a new VariableNode
    if(pImpl_->opNodesVector_.end() == iter)
    {
        int counter = pImpl_->opNodesVector_.size();
        increment(counter);

        OperatorNode *opNode = new OperatorNotNode(*this, operand, counter, tmp_label);

        if(opNode)
        {
            pImpl_->opNodesVector_.push_back(opNode);
            pImpl_->opNodesCnt_.push_back(1);
        }
        else
        {
            throw FormulaParserException("Failed in creating OperatorNotNode!");
        }

        return *opNode;
    }
    else
    {
        ++(pImpl_->opNodesCnt_[(*iter)->getIndex() - 1]);
        // ugly hack!
        assert((*iter)->getChild(0)->getNodeType() == VAR_NODE);
        ++(pImpl_->varNodesCnt_[(*iter)->getChild(0)->getIndex() - 1]);
        return *(*iter);
    }
    */

    int counter = pImpl_->opNodesVector_.size();
    increment(counter);

    OperatorNode *opNode = new OperatorNotNode(*this, operand, counter, label);

    if(opNode)
    {
        pImpl_->opNodesVector_.push_back(opNode);
        pImpl_->opNodesCnt_.push_back(1);
    }
    else
    {
        throw FormulaParserException("Failed in creating OperatorNotNode!");
    }

    return *opNode;
}
 
 
FormulaNode& FormulaNodeFactory::createClauseNode(const std::vector<FormulaNode *>& literalVector,
                                                  const std::string& label)
{
    FormulaNode& node = createClauseNode(label);

    for(unsigned int i = 0; i < literalVector.size(); ++i)
    {
        node.addChildNode(*(literalVector[i]));
    }

    return node;
}


FormulaNode& FormulaNodeFactory::createClauseNode(const std::string& label)
{
    int counter = pImpl_->opNodesVector_.size();
    increment(counter);
    
    ClauseNode *node = new ClauseNode(*this, counter, label);

    if(node)
    {
        pImpl_->opNodesVector_.push_back(node);
        pImpl_->opNodesCnt_.push_back(1);
    }
    else
    {
        throw FormulaParserException("Failed in creating ClauseNode!");
    }

    return *node;
}


FormulaNode& FormulaNodeFactory::createCNFNode(const std::vector<FormulaNode *>& clauseVector,
                                               const std::string& label)
{
    FormulaNode& node = createCNFNode(label);

    for(unsigned int i = 0; i < clauseVector.size(); ++i)
    {
        node.addChildNode(*(clauseVector[i]));
    }

    return node;
}


FormulaNode& FormulaNodeFactory::createCNFNode(const std::string& label)
{
    int counter = pImpl_->opNodesVector_.size();
    increment(counter);

    CNFNode *node = new CNFNode(*this, counter, label);

    if(node)
    {
        pImpl_->opNodesVector_.push_back(node);
        pImpl_->opNodesCnt_.push_back(1);
    }
    else
    {
        throw FormulaParserException("Failed in creating ClauseNode!");
    }

    return *node;   
}

/*
void FormulaNodeFactory::removeFormulaNodeRecur(FormulaNode& node)
{
    removeFormulaNode(node);

    if(node.getNodeType() != VAR_NODE &&
       node.getNodeType() != TRUE_NODE &&
       node.getNodeType() != FALSE_NODE)
    {
        // CRUDE! Really should use smart pointers
   //     if(!(pImpl_->opNodesCnt_[node.getIndex() - 1]))
     //   {
            for(int i = 0; i < pImpl_->opNodesVector_[node.getIndex() - 1]->getNumOfChildren(); ++i)
            {
                removeFormulaNode(*(pImpl_->opNodesVector_[node.getIndex() - 1]->getChild(i)));
            }
       // }
    }
}
*/

bool FormulaNodeFactory::removeFormulaNode(FormulaNode& node)
{
//std::cerr << "remove node: node type: " << node.getNodeType() << " index: " << node.getIndex() << std::endl;
//std::cerr << "varNodesCnt_: ";
//printVector(pImpl_->varNodesCnt_);
//std::cerr << std::endl << "opNodesCnt_: ";
//printVector(pImpl_->opNodesCnt_);
//std::cerr << std::endl;


    if(node.getNodeType() == VAR_NODE)
    {
        if(pImpl_->varNodesCnt_.size() < node.getIndex())
            return false;

        assert(pImpl_->varNodesCnt_[node.getIndex() - 1] > 0);
        --(pImpl_->varNodesCnt_[node.getIndex() - 1]);
    }
    else if(node.getNodeType() == TRUE_NODE || node.getNodeType() == FALSE_NODE)
    {
        return true;
    }
    else
    {
        if(pImpl_->opNodesCnt_.size() < node.getIndex())
            return false;

        assert(pImpl_->opNodesCnt_[node.getIndex() - 1] > 0);
        --(pImpl_->opNodesCnt_[node.getIndex() - 1]);
    }

    return true;
}


void FormulaNodeFactory::freeMemory()
{
//std::cerr << "varNodesCnt_: ";
//printVector(pImpl_->varNodesCnt_);
//std::cerr << std::endl << "opNodesCnt_: ";
//printVector(pImpl_->opNodesCnt_);
//std::cerr << std::endl;
    processNodesPtrs(pImpl_->varNodesVector_, pImpl_->varNodesCnt_);
    processNodesPtrs(pImpl_->opNodesVector_, pImpl_->opNodesCnt_);
}


Size_Type FormulaNodeFactory::getNumOfVar() const
{
    return pImpl_->varNodesVector_.size();
}


std::string FormulaNodeFactory::getVarName(const Index_t& index) const
{
    return this->pImpl_->varNodesVector_[index - 1]->getLabel();
}


Index_t FormulaNodeFactory::getVarIndex(const std::string& varLabel) const
{
    std::vector<VariableNode *>::const_iterator iter =
            this->pImpl_->varNodesVector_.begin();
    std::vector<VariableNode *>::const_iterator ITER_END =
            this->pImpl_->varNodesVector_.end();

    for(; iter != ITER_END; ++iter)
    {
        std::string currLabel = (*iter)->getLabel();

        if(varLabel == currLabel)
        {
            return (*iter)->getIndex();
        }
    }

    return 0;
}


void FormulaNodeFactory::clear()
{
    pImpl_->deleteNodes();
}


unsigned int FormulaNodeFactory::totalNumOfNodes() const
{
    unsigned int num = 2;

    for(unsigned int i = 0; i < pImpl_->varNodesCnt_.size(); ++i)
    {
        num += pImpl_->varNodesCnt_[i];
    }
    for(unsigned int i = 0; i < pImpl_->opNodesCnt_.size(); ++i)
    {
        num += pImpl_->opNodesCnt_[i];
    }

//std::cerr << "op nodes: ";
//printVector2(pImpl_->opNodesVector_);
//std::cerr << std::endl << "var nodes: ";
//printVector2(pImpl_->varNodesVector_);
//std::cerr << std::endl;
    return num;
}


// ------------------------
// PImpl
// ------------------------
FormulaNodeFactory::PImpl::PImpl(FormulaNodeFactory& formulaNodeFactory) :
        intermediateVarCnt_(0),
        formulaNodeFactory_(formulaNodeFactory)
{
    truefalseNodePtr_[TRUE_NODE] = new TrueNode(formulaNodeFactory_);
    truefalseNodePtr_[FALSE_NODE] = new FalseNode(formulaNodeFactory_);
}


FormulaNodeFactory::PImpl::~PImpl()
{
    deleteNodes(); 
    delete truefalseNodePtr_[FALSE_NODE];
    delete truefalseNodePtr_[TRUE_NODE];
}


std::string FormulaNodeFactory::PImpl::generateNewLabel()
{
    std::ostringstream convert;   // stream used for the conversion
    convert << WIRE_LABEL << intermediateVarCnt_;
    ++intermediateVarCnt_;

    return convert.str();
}


void FormulaNodeFactory::PImpl::deleteNodes()
{
    std::for_each(opNodesVector_.begin(), opNodesVector_.end(), DeleteNodeFunctor());
    std::for_each(varNodesVector_.begin(), varNodesVector_.end(), DeleteNodeFunctor());
    
    opNodesVector_.clear();
    varNodesVector_.clear();
    opNodesCnt_.clear();
    varNodesCnt_.clear();
    
    intermediateVarCnt_ = 0;
}

    
template<typename T1, typename T2>
void processNodesPtrs(T1& nodePtrs, T2& nodeCnts)
{
    assert(nodePtrs.size() == nodeCnts.size());

    typename T1::iterator nodePtrsIter = nodePtrs.begin(); 
    typename T2::iterator nodeCntsIter = nodeCnts.begin();
    Size_Type new_index = 1;
    
    while(nodeCntsIter != nodeCnts.end())
    {
        if(*nodeCntsIter == 0)
        {
            delete *nodePtrsIter;
            nodePtrsIter = nodePtrs.erase(nodePtrsIter);
            nodeCntsIter = nodeCnts.erase(nodeCntsIter);
        }
        else
        {
            (*nodePtrsIter)->setIndex(new_index);
            ++new_index;
            ++nodePtrsIter;
            ++nodeCntsIter;
        }
    }
}

} // namespace QBFEquiChecker_NS

