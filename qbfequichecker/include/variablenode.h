/**
 * @file variablenode.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef VARIABLENODE_H
#define VARIABLENODE_H


#include "literalnode.h"
#include "qbfequichecker_common.h"
#include <iosfwd>


namespace QBFEquiChecker_NS
{

class FormulaNodeFactory;

/**
 * @class VariableNode
 *
 * VariableNode represents both a variable and a boolean value (i.e. true or false).
 * For fixed boolean value, the index will be assigned as 0
 *
 */
class VariableNode : public LiteralNode
{
    public:
        /**
         * If VariableNode is representing a boolean value, then the parameter index should be 0
         */
        explicit VariableNode(FormulaNodeFactory& formulaNodeFactory,
                              const std::string& label,
                              const Logic_t::LOGIC_TYPE_E_& value,
                              const Index_t& index,
                              bool isNegated = false);
        explicit VariableNode(FormulaNodeFactory& formulaNodeFactory,
                              const std::string& label,
                              bool value,
                              const Index_t& index,
                              bool isNegated = false);
        virtual ~VariableNode();
      
        VariableNode(const VariableNode& rhs);
        VariableNode& operator=(const VariableNode& rhs);
        

        void swap(VariableNode& rhs);


		/**
		 * A covariant return type virtual function.
         * An idiom for duplicating the derived classes; uses copy constructor
		 */
		virtual VariableNode *clone() const;



    private:
        virtual Logic_t evaluate_();
        virtual void print_(std::ostream& outStrm) const;
        virtual Node_Type getNodeType_() const;
        virtual std::string findLabel_(const Index_t& index) const;
        virtual Index_t findVarIndex_(const std::string& varName) const;
        virtual bool setVariableVal_(const Index_t& index, const Logic_t& value);

        Logic_t value_;
};


} // namespace QBFEquiChecker_NS


namespace std
{
    template<>
    void swap<QBFEquiChecker_NS::VariableNode>(
              QBFEquiChecker_NS::VariableNode& lhs,
              QBFEquiChecker_NS::VariableNode& rhs);
}


#endif // VARIABLENODE_H

