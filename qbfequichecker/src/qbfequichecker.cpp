/**
 * @file qbfequichecker.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#include "qbfequichecker.h"
#include "formulaparser.h"
#include "formulanode.h"
#include "formulanodefactory.h"
#include "solveradaptorbase.h"
#include "solveradaptor.h"
#include "depqbfadaptor.h"
#include "logic_t.h"

#include <list>
#include <algorithm>
#include <ostream>
#include <fstream>
#include <utility> // std::pair
#include <ctime>

#include <iostream>


namespace QBFEquiChecker_NS
{

const char *QBFEquiChecker::NIL_FORMULA = "Nil Formula";


struct QBFEquiChecker::PImpl_ : private Uncopyable
{
    typedef std::list< std::pair<Index_t, Quantifier_Type> > QUANT_VAR_LIST_T;

    PImpl_() :
        rootNode_(NULL),
        cnfRoot_(NULL),
        parser_(formulaNodeFactory_),
        solving_time_(0)
    {
    }
    
    ~PImpl_()
    {
        
    }
    

    bool swapSetQuantVar(QUANT_VAR_LIST_T::iterator target_pos,
                         const Index_t& index,
                         const Quantifier_Type& quant)
    {
        if(index == 0)
            return false;

        QUANT_VAR_LIST_T::const_iterator END_ = quantVarList_.end();
        for(QUANT_VAR_LIST_T::iterator iter = quantVarList_.begin();
            iter != END_;
            ++iter)
        {
            if(iter->first == index)
            {
                iter->second = quant;
                quantVarList_.splice(target_pos, quantVarList_, iter);

                return true;
            }
        }

        return false;
    }


    bool isRootCNF() const
    {
        if(rootNode_ == NULL || rootNode_->getNodeType() != CNF_NODE)
            return false;
        return true;
    }

    FormulaNodeFactory formulaNodeFactory_;
    FormulaNode *rootNode_;
    FormulaNode *cnfRoot_;
    std::string iscasStr_;

    FormulaParser parser_;
    std::list< std::pair<Index_t, Quantifier_Type> > quantVarList_;

    // after solving
    std::map<Index_t, Logic_t> varValues_;
    double solving_time_;
};


QBFEquiChecker::QBFEquiChecker() :
    pImpl_(new PImpl_)
{
}


QBFEquiChecker::~QBFEquiChecker()
{
    delete pImpl_;
}


bool QBFEquiChecker::moveToOuterUniQuantifier(const Index_t& index)
{
    return this->pImpl_->swapSetQuantVar(
            this->pImpl_->quantVarList_.begin(),
            index,
            FOR_ALL);
}


bool QBFEquiChecker::moveToOuterUniQuantifier(const std::string& varName)
{
    return this->pImpl_->swapSetQuantVar(
            this->pImpl_->quantVarList_.begin(),
            this->pImpl_->rootNode_->findVarIndex(varName),
            FOR_ALL);
}


bool QBFEquiChecker::moveToOuterExtQuantifier(const Index_t& index)
{
    return this->pImpl_->swapSetQuantVar(
            this->pImpl_->quantVarList_.begin(),
            index,
            EXISTS);
}



bool QBFEquiChecker::moveToOuterExtQuantifier(const std::string& varName)
{
    return this->pImpl_->swapSetQuantVar(
            this->pImpl_->quantVarList_.begin(),
            this->pImpl_->rootNode_->findVarIndex(varName),
            EXISTS);
}


bool QBFEquiChecker::moveToInnerUniQuantifier(const Index_t& index)
{
    return this->pImpl_->swapSetQuantVar(
            this->pImpl_->quantVarList_.end(),
            index,
            FOR_ALL);
}


bool QBFEquiChecker::moveToInnerUniQuantifier(const std::string& varName)
{
    return this->pImpl_->swapSetQuantVar(
            this->pImpl_->quantVarList_.end(),
            this->pImpl_->rootNode_->findVarIndex(varName),
            FOR_ALL);
}


bool QBFEquiChecker::moveToInnerExtQuantifier(const Index_t& index)
{
    return this->pImpl_->swapSetQuantVar(
            this->pImpl_->quantVarList_.end(),
            index,
            EXISTS);
}


bool QBFEquiChecker::moveToInnerExtQuantifier(const std::string& varName)
{
    return this->pImpl_->swapSetQuantVar(
            this->pImpl_->quantVarList_.end(),
            this->pImpl_->rootNode_->findVarIndex(varName),
            EXISTS);
}


void QBFEquiChecker::resetQuantifier()
{
    QBFEquiChecker::PImpl_::QUANT_VAR_LIST_T::const_iterator END_ =
            this->pImpl_->quantVarList_.end();
    for(QBFEquiChecker::PImpl_::QUANT_VAR_LIST_T::iterator iter = this->pImpl_->quantVarList_.begin();
        iter != END_;
        ++iter)
    {
        iter->second = EXISTS;
    }
}


std::string QBFEquiChecker::getVarName(const Index_t& index) const
{
    return pImpl_->rootNode_->findLabel(index);
}


Index_t QBFEquiChecker::getVarIndex(const std::string& varName) const
{
    return this->pImpl_->formulaNodeFactory_.getVarIndex(varName);
}

    
Size_Type QBFEquiChecker::getNumOfVar() const
{
    return pImpl_->formulaNodeFactory_.getNumOfVar();
}


//Index_t QBFEquiChecker::getVarIndex(const std::string& varName) const
//{
//}


//void QBFEquiChecker::createMiter(const std::string& formula1,
//                                 const std::string& formula2)
//{
//    pImpl_->formulaNodeFactory_.clear();
//
//    FormulaNode *f1Node = pImpl_->parser_.parse(formula1);
//    FormulaNode *f2Node = pImpl_->parser_.parse(formula2);
//
//
//    // consider the case where there is empty formula!!
//    if(f1Node != NULL)
//    {
//        f1Node = &(f1Node->simplify());
//        pImpl_->formulaNodeFactory_.freeMemory();
//    }
//
//    if(f2Node != NULL)
//    {
//        f2Node = &(f2Node->simplify());
//        pImpl_->formulaNodeFactory_.freeMemory();
//    }
//
//    FormulaNode& cnf = pImpl_->formulaNodeFactory_.createCNFNode();
//    FormulaNode& a = f1Node->convertCNF(cnf);
//    FormulaNode& b = f2Node->convertCNF(cnf);
//    FormulaNode& c = pImpl_->formulaNodeFactory_.createVariableNode();
//
//    // cannot simplify!
//    FormulaNode *n_a_ptr = NULL;
//    if(a.getNodeType() == OP_NOT_NODE)
//    {
//        n_a_ptr = a.getChild(0);
//    }
//    else
//    {
//        n_a_ptr = &(pImpl_->formulaNodeFactory_.createOperatorNotNode(a));
//    }
//    FormulaNode *n_b_ptr = NULL;
//    if(b.getNodeType() == OP_NOT_NODE)
//    {
//        n_b_ptr = b.getChild(0);
//    }
//    else
//    {
//        n_b_ptr = &(pImpl_->formulaNodeFactory_.createOperatorNotNode(b));
//    }
//
//    FormulaNode& n_c =
//            pImpl_->formulaNodeFactory_.createOperatorNotNode(c);
//
//
//    // 1) create the nxor gate
//    // c = a ^ b
//    //      => (~a | ~b | ~c) & (a | b | ~c) & (a | ~b | c) & (~a | b | c)
//    // 2) add the not gate
//
//    std::vector<FormulaNode *>clause1;
//    clause1.push_back(n_a_ptr);
//    clause1.push_back(n_b_ptr);
//    clause1.push_back(&n_c);
//
//    std::vector<FormulaNode *>clause2;
//    clause2.push_back(&a);
//    clause2.push_back(&b);
//    clause2.push_back(&n_c);
//
//    std::vector<FormulaNode *>clause3;
//    clause3.push_back(&a);
//    clause3.push_back(n_b_ptr);
//    clause3.push_back(&c);
//
//    std::vector<FormulaNode *>clause4;
//    clause4.push_back(n_a_ptr);
//    clause4.push_back(&b);
//    clause4.push_back(&c);
//
//    cnf.addChildNode(pImpl_->formulaNodeFactory_.createClauseNode(clause1));
//    cnf.addChildNode(pImpl_->formulaNodeFactory_.createClauseNode(clause2));
//    cnf.addChildNode(pImpl_->formulaNodeFactory_.createClauseNode(clause3));
//    cnf.addChildNode(pImpl_->formulaNodeFactory_.createClauseNode(clause4));
//
//    std::vector<FormulaNode *>clause5;
//    if(c.getNodeType() == OP_NOT_NODE)
//    {
//        clause5.push_back(&c);
//    }
//    else
//    {
//        FormulaNode& output = (pImpl_->formulaNodeFactory_.createOperatorNotNode(c));
//        clause5.push_back(&output);
//    }
//
//    cnf.addChildNode(pImpl_->formulaNodeFactory_.createClauseNode(clause5));
//
//    pImpl_->rootNode_ = &cnf;
//}

    
void QBFEquiChecker::printIndexVarMap(std::ostream& outstream) const
{
    // node index is always 1 + INDEX of the vector!
    //return pImpl_->varNodePtr_[index - 1]->getLabel();
    
    outstream << "Index\tVariable Name\n";
    
    if(pImpl_->rootNode_ != NULL)
    {
        for(Size_Type i = 1; i <= pImpl_->formulaNodeFactory_.getNumOfVar(); ++i)
        {
            //std::string label = pImpl_->cnfRoot_->findLabel(i);
            std::string label = pImpl_->formulaNodeFactory_.getVarName(i);

            if(label.length() > 0)
                outstream << i << "\t" << label << std::endl;
        }

    }
}
    

void QBFEquiChecker::printFormula(std::ostream& outStrm) const
{
    if(pImpl_->rootNode_ != NULL)
        outStrm << *(pImpl_->rootNode_);
    else
        outStrm << NIL_FORMULA;
}
    
    
void QBFEquiChecker::printCNF(std::ostream& outStrm) const
{
    if(pImpl_->cnfRoot_ != NULL)
        outStrm << *(pImpl_->cnfRoot_);
    else
        outStrm << "Not converted to CNF yet!";
}
    
    
void QBFEquiChecker::printDIMACS(std::ostream& outStrm) const
{
    if(pImpl_->cnfRoot_ == NULL)
    {
        return;
    }

    // c example:
    // p cnf 4 2
    // a 1 3 4 0
    // e 2 0
    // -1 2 0
    // 2 -3 -4 0
        
    outStrm << "c Generated by QBFEquiChecker" << std::endl;
    outStrm << "p cnf " << pImpl_->formulaNodeFactory_.getNumOfVar()
            << " " << pImpl_->cnfRoot_->getNumOfChildren() << std::endl;
        
    QBFEquiChecker::PImpl_::QUANT_VAR_LIST_T::const_iterator ITER_END =
            this->pImpl_->quantVarList_.end();
    QBFEquiChecker::PImpl_::QUANT_VAR_LIST_T::iterator iter =
            this->pImpl_->quantVarList_.begin();
    Quantifier_Type prev_quant = iter->second;

    if(prev_quant == EXISTS)
    {
        outStrm << "e ";
    }
    else
    {
        outStrm << "a ";
    }

    outStrm << iter->first << " ";

    for(++iter; iter != ITER_END; ++iter)
    {
        if(iter->second != prev_quant)
        {
            if(iter->second == EXISTS)
            {
                outStrm << "0\ne ";
            }
            else
            {
                outStrm << "0\na ";
            }
        }

        outStrm << iter->first << " ";
        prev_quant = iter->second;
    }

    outStrm << "0\n";


//    if(pImpl_->uniQuantVarVec_.size() > 0)
//    {
//        // sort the vector first
//        std::sort(pImpl_->uniQuantVarVec_.begin(), pImpl_->uniQuantVarVec_.end());
//        std::vector<Index_t> extQuantVarVec;
//
//        outStrm << "a ";
//        Index_t index_a = 0;
//        Index_t index_e = 1;
//        for(index_a = 0; index_a < pImpl_->uniQuantVarVec_.size(); ++index_a)
//        {
//            Index_t curr_a_index = pImpl_->uniQuantVarVec_[index_a];
//            outStrm << curr_a_index << " ";
//
//            for(; index_e < curr_a_index; ++index_e)
//            {
//                extQuantVarVec.push_back(index_e);
//            }
//            ++index_e;
//        }
//        for(;index_e <= pImpl_->formulaNodeFactory_.getNumOfVar(); ++index_e)
//        {
//            extQuantVarVec.push_back(index_e);
//        }
//
//        outStrm << "0\ne ";
//
//        for(unsigned int i = 0; i < extQuantVarVec.size(); ++i)
//        {
//            outStrm << extQuantVarVec[i] << " ";
//        }
//        outStrm << "0\n";
//    }
                
    for(Size_Type i = 0; i < pImpl_->cnfRoot_->getNumOfChildren(); ++i)
    {
        FormulaNode *clause = pImpl_->cnfRoot_->getChild(i);
        
        if(!clause->getNumOfChildren())
        {
            outStrm << clause->getIndex() << " ";
        }
        else
        {
            for(Size_Type j = 0; j < clause->getNumOfChildren(); ++j)
            {
                FormulaNode *literalNode = clause->getChild(j);
                    
                if(literalNode->getNodeType() == OP_NOT_NODE)
                {
                    outStrm << "-" << literalNode->getChild(0)->getIndex() << " ";
                }
                else
                {
                    outStrm << literalNode->getIndex() << " ";
                }
            }
        }
           
        outStrm << "0" << std::endl;
    }
}
    

unsigned int QBFEquiChecker::totalNumOfNodes() const
{
    return pImpl_->formulaNodeFactory_.totalNumOfNodes();
}


bool QBFEquiChecker::parse(const std::string& exprStr)
{
    pImpl_->formulaNodeFactory_.clear();
    pImpl_->rootNode_ = NULL;
    pImpl_->cnfRoot_  = NULL;
    pImpl_->iscasStr_.clear();
    pImpl_->quantVarList_.clear();

    try
    {
        pImpl_->rootNode_ = pImpl_->parser_.parse(exprStr);

        // push all variable nodes into the quantifier list
        for(Index_t index = 1; index <= pImpl_->formulaNodeFactory_.getNumOfVar(); ++index)
        {
            pImpl_->quantVarList_.push_back(std::make_pair(index, EXISTS));
        }

    } catch(FormulaParserException& ex)
    {
        std::cerr << "EXCEPTION: " << ex.what() << std::endl;
        return false;
    }

    return true;
}
    
    
bool QBFEquiChecker::convertCNF()
{
    if(pImpl_->rootNode_ == NULL || pImpl_->cnfRoot_ != NULL || pImpl_->iscasStr_.length() > 0)
        return false;

    simplify();

    Size_Type before_cnf_index = this->pImpl_->formulaNodeFactory_.getNumOfVar();

    FormulaNode& cnfNode = pImpl_->formulaNodeFactory_.createCNFNode();
    FormulaNode& output = pImpl_->rootNode_->convertCNF(cnfNode);

    FormulaNode& clauseNode = pImpl_->formulaNodeFactory_.createClauseNode();
    clauseNode.addChildNode(output);
    cnfNode.addChildNode(clauseNode);
    pImpl_->cnfRoot_ = &cnfNode;

    // add intermediate variables
    Size_Type after_cnf_index = this->pImpl_->formulaNodeFactory_.getNumOfVar();

    for(++before_cnf_index; before_cnf_index <= after_cnf_index; ++before_cnf_index)
    {
        this->pImpl_->quantVarList_.push_back(std::make_pair(before_cnf_index, EXISTS));
    }

    return true;
}
    

std::string QBFEquiChecker::convertISCAS()
{
    if(pImpl_->iscasStr_.length() > 0)
        return pImpl_->iscasStr_;

    if(pImpl_->rootNode_ == NULL || pImpl_->cnfRoot_ != NULL)
        return "";

    simplify();

    std::string iscasStr;
    std::string quantStr;
    std::string ioStr;


    QBFEquiChecker::PImpl_::QUANT_VAR_LIST_T::const_iterator ITER_END =
                this->pImpl_->quantVarList_.end();

    for(QBFEquiChecker::PImpl_::QUANT_VAR_LIST_T::iterator iter = this->pImpl_->quantVarList_.begin();
        iter != ITER_END;
        ++iter)
    {
        std::string label = pImpl_->rootNode_->findLabel(iter->first);

        if(iter->second == EXISTS)
        {
            quantStr += "EXISTS( " + label + " )\n";
        }
        else
        {
            quantStr += "FORALL( " + label + " )\n";
        }

        ioStr += "INPUT( " + label + " )\n";
    }

//    for(;index_e <= pImpl_->formulaNodeFactory_.getNumOfVar(); ++index_e)
//    {
//        extQuantVarVec.push_back(index_e);
//    }
//
//    for(unsigned int i = 0; i < extQuantVarVec.size(); ++i)
//    {
//        std::string label = pImpl_->rootNode_->findLabel(extQuantVarVec[i]);
//
//        quantStr += "EXISTS( " + label + " )\n";
//        ioStr += "INPUT( " + label + " )\n";
//    }


    FormulaNode& outNode = pImpl_->rootNode_->convertISCAS(iscasStr);
    ioStr += "OUTPUT( " + outNode.getLabel() + " )\n";

    pImpl_->iscasStr_ = quantStr + ioStr + iscasStr;

    return pImpl_->iscasStr_;
}


void QBFEquiChecker::simplify()
{
    if(pImpl_->rootNode_ != NULL)
    {
        pImpl_->rootNode_ = &(pImpl_->rootNode_->simplify());
//WATCH OUT
        pImpl_->formulaNodeFactory_.freeMemory();
    }
}
    

#ifdef USE_SOLVER_DEPQBF
SolverResult_Type QBFEquiChecker::solve() const
{
    SolverAdaptor<DepQBFAdaptor> solver;
    return this->solve(solver);
}
#endif // USE_SOLVER_DEPQBF


void QBFEquiChecker::solve(SolverAdaptorBase& solver, const char *testFile) const
{
    assert(pImpl_ != 0);
    pImpl_->varValues_.clear();

    std::ofstream outFileStream (testFile);

    if(this->pImpl_->cnfRoot_ != NULL)
    {
        this->printDIMACS(outFileStream);
    }
    else if(this->pImpl_->iscasStr_.length() > 0)
    {
        outFileStream << this->pImpl_->iscasStr_;
    }

    outFileStream.close();

    time_t t1 = time(NULL);
    solver.solve(testFile);
    time_t t2 = time(NULL);
    this->pImpl_->solving_time_ = difftime(t1, t2);
}


SolverResult_Type QBFEquiChecker::solve(SolverAdaptorBase& solver) const
{
    SolverResult_Type result = SOLVER_UNKNOWN;

    pImpl_->varValues_.clear();

    if(this->pImpl_->cnfRoot_ != NULL)
    {
        time_t t1 = time(NULL);
        result = solver.solve(*(this->pImpl_->cnfRoot_),
                              this->pImpl_->quantVarList_,
                              this->pImpl_->formulaNodeFactory_.getNumOfVar(),
                              this->pImpl_->varValues_);
        time_t t2 = time(NULL);

        this->pImpl_->solving_time_ = difftime(t2, t1);
    }
    else if(this->pImpl_->iscasStr_.length() > 0)
    {
        time_t t1 = time(NULL);
        result = solver.solve(*(this->pImpl_->rootNode_),
                              this->pImpl_->quantVarList_,
                              this->pImpl_->formulaNodeFactory_.getNumOfVar(),
                              this->pImpl_->varValues_);
        time_t t2 = time(NULL);

        this->pImpl_->solving_time_ = difftime(t2, t1);
    }

    return result;
}


double QBFEquiChecker::timeTakenForSolving() const
{
    return this->pImpl_->solving_time_;
}


void QBFEquiChecker::printVarAssignments(std::ostream& outstream) const
{
    std::map<Index_t, Logic_t>::const_iterator iter =
            this->pImpl_->varValues_.begin();
    std::map<Index_t, Logic_t>::const_iterator ITER_END =
                this->pImpl_->varValues_.end();

    for(; iter != ITER_END; ++iter)
    {
        outstream << this->pImpl_->formulaNodeFactory_.getVarName(iter->first)
                  << "\t\t" << iter->second << std::endl;
    }
}


Logic_t QBFEquiChecker::getVarAssignment(const Index_t& index) const
{
    std::map<Index_t, Logic_t>::const_iterator iter =
            this->pImpl_->varValues_.find(index);

    if(iter == this->pImpl_->varValues_.end())
        return Logic_t(Logic_t::SYMBOLIC);

    return iter->second;
}


bool QBFEquiChecker::setVariableValue(Index_t index, const Logic_t& val)
{
    return this->pImpl_->rootNode_->setVariableValue(index, val);
}


bool QBFEquiChecker::setVariableValue(Index_t index, const Logic_t::LOGIC_TYPE_E_& val)
{
    return this->pImpl_->rootNode_->setVariableValue(index, val);
}


bool QBFEquiChecker::setVariableValue(Index_t index, bool val)
{
    if(this->pImpl_->cnfRoot_)
    {
        return this->pImpl_->cnfRoot_->setVariableValue(index, val);
    }
    else if(this->pImpl_->rootNode_)
    {
        return this->pImpl_->rootNode_->setVariableValue(index, val);
    }
    else
    {
        return false;
    }
}


Logic_t QBFEquiChecker::evaluate()
{
    if(this->pImpl_->cnfRoot_)
    {
        return this->pImpl_->cnfRoot_->evaluate();
    }
    else if(this->pImpl_->rootNode_)
    {
        return this->pImpl_->rootNode_->evaluate();
    }
    else
    {
        return Logic_t(Logic_t::SYMBOLIC);
    }

}
    
} // namespace QBFEquiChecker_NS


