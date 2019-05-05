/**
 * @file truenode.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */



#ifndef TRUENODE_H
#define TRUENODE_H


#include "literalnode.h"
#include "qbfequichecker_common.h"
#include <iosfwd>
#include <string>


namespace QBFEquiChecker_NS
{

/**
 * @class TrueNode
 *
 * TrueNode represents the logic true
 *
 */
class TrueNode : public LiteralNode
{
    public:
        /**
         * If TrueNode is representing a boolean value, then the parameter index should be 0
         */
        explicit TrueNode(FormulaNodeFactory& formulaNodeFactory,
                          const Index_t& index = 0);
        virtual ~TrueNode();
      
        TrueNode(const TrueNode& rhs);
        TrueNode& operator=(const TrueNode& rhs);
        

        void swap(TrueNode& rhs);


		/**
		 * A covariant return type virtual function.
         * An idiom for duplicating the derived classes; uses copy constructor
		 */
		virtual TrueNode *clone() const;


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
    void swap<QBFEquiChecker_NS::TrueNode>(
              QBFEquiChecker_NS::TrueNode& lhs,
              QBFEquiChecker_NS::TrueNode& rhs);
}


#endif // TRUENODE_H

