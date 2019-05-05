/**
 * @file literalnode.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef LITERALNODE_H
#define LITERALNODE_H


#include "formulanode.h"
#include "qbfequichecker_common.h"
#include <iosfwd>
#include <string>
#include <vector>
#include <string>

namespace QBFEquiChecker_NS
{

/**
 * @class LiteralNode
 *
 * LiteralNode represents both a variable and a boolean value (i.e. true or false).
 * For fixed boolean value, the index will be assigned as 0
 *
 */
class LiteralNode : public FormulaNode
{
    public:
        /**
         * If LiteralNode is representing a boolean value, then the parameter index should be 0
         */
        explicit LiteralNode(FormulaNodeFactory& formulaNodeFactory,
                             const Index_t& index,
                             const std::string& label,
                             bool isNegated);
        virtual ~LiteralNode() = 0;
   
   
		/**
		 * A covariant return type virtual function.
         * An idiom for duplicating the derived classes; uses copy constructor
		 */
		virtual LiteralNode *clone() const = 0;


        /**
         * @return true if negated; false otherwise
         */
        inline bool isNegated() const
        {
            return isNegated_;
        }


    protected:
        LiteralNode(const LiteralNode& rhs);
        LiteralNode& operator=(const LiteralNode& rhs);
        
        void swap(LiteralNode& rhs);


    private:
        virtual Logic_t evaluate_() = 0;
        virtual void print_(std::ostream& outStrm) const = 0;
        virtual Node_Type getNodeType_() const = 0;
        virtual std::string findLabel_(const Index_t& index) const = 0;
        virtual Index_t findVarIndex_(const std::string& varName) const = 0;
        virtual Size_Type getTreeHeight_() const;
        virtual bool setVariableVal_(const Index_t& index, const Logic_t& value) = 0;
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

        bool isNegated_;
};


} // namespace QBFEquiChecker_NS


#endif // LITERALNODE_H

