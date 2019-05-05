/**
 * @file cnfnode.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */



#ifndef CNFNODE_H
#define CNFNODE_H


#include "formulanode.h"
#include "qbfequichecker_common.h"
#include <algorithm>
#include <iosfwd>
#include <vector>
#include <string>

namespace QBFEquiChecker_NS
{

class FormulaNodeFactory;

/**
 * @class CNFNode
 *
 * Models a CNF
 */
class CNFNode : public FormulaNode
{
    public:
        explicit CNFNode(FormulaNodeFactory& formulaNodeFactory,
                         const Index_t& index = 0,
                         const std::string& label = "");
        virtual ~CNFNode();
    	CNFNode(const CNFNode& rhs);
        CNFNode& operator=(const CNFNode& rhs);

        /// swap
        void swap(CNFNode& rhs);


		/**
		 * A covariant return type virtual function.
         * An idiom for duplicating the derived classes; uses copy constructor
		 */
		virtual CNFNode *clone() const;


    private:
        virtual Logic_t evaluate_();
        virtual void print_(std::ostream& outStrm) const;
        virtual Size_Type getTreeHeight_() const;
        virtual Node_Type getNodeType_() const;
        virtual std::string findLabel_(const Index_t& index) const;
        virtual Index_t findVarIndex_(const std::string& varName) const;
        virtual bool setVariableVal_(const Index_t&, const Logic_t&);
        virtual FormulaNode *getChild_(unsigned int childIndex);
        virtual const FormulaNode *getChild_(unsigned int childIndex) const;
        virtual Size_Type getNumOfChildren_() const;
        virtual bool addChildNode_(FormulaNode& childNode);
        virtual bool setChild_(FormulaNode& childNode, unsigned int index);
        virtual FormulaNode& simplify_();
        virtual void removeAllChildrenNode_();
        virtual bool removeChildNode_(unsigned int childIndex);
        virtual void deleteTree_();
        virtual FormulaNode& convertCNF_(FormulaNode& cnfNode);
        virtual FormulaNode& convertISCAS_(std::string& outStr);
        
        std::vector<FormulaNode *> childrenNodes_;
};


} // namespace QBFEquiChecker_NS


namespace std
{
    template<>
    void swap<QBFEquiChecker_NS::CNFNode>(
              QBFEquiChecker_NS::CNFNode& lhs,
              QBFEquiChecker_NS::CNFNode& rhs);
}


#endif // CNFNODE_H

