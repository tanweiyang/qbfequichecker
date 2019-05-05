/**
 * @file falsenode.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef FALSENODE_H
#define FALSENODE_H


#include "literalnode.h"
#include "qbfequichecker_common.h"
#include <iosfwd>
#include <string>


namespace QBFEquiChecker_NS
{

class FormulaNodeFactory;

/**
 * @class FalseNode
 *
 * FalseNode represents the logic true
 *
 */
class FalseNode : public LiteralNode
{
    public:
        /**
         * If FalseNode is representing a boolean value, then the parameter index should be 0
         */
        explicit FalseNode(FormulaNodeFactory& formulaNodeFactory,
                           const Index_t& index = 0);
        virtual ~FalseNode();
      
        FalseNode(const FalseNode& rhs);
        FalseNode& operator=(const FalseNode& rhs);
        

        void swap(FalseNode& rhs);


		/**
		 * A covariant return type virtual function.
         * An idiom for duplicating the derived classes; uses copy constructor
		 */
		virtual FalseNode *clone() const;


    private:
        virtual Logic_t evaluate_();
        virtual void print_(std::ostream& outStrm) const;
        virtual Node_Type getNodeType_() const;
        virtual std::string findLabel_(const Index_t& index) const;
        virtual Index_t findVarIndex_(const std::string& varName) const;
        virtual bool setVariableVal_(const Index_t& index, const Logic_t& value);
};


} // namespace QBFEquiChecker_NS


namespace std
{
    template<>
    void swap<QBFEquiChecker_NS::FalseNode>(
              QBFEquiChecker_NS::FalseNode& lhs,
              QBFEquiChecker_NS::FalseNode& rhs);
}


#endif // FALSENODE_H

