/**
 * @file operatorornode.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef OPERATORORNODE_H
#define OPERATORORNODE_H


#include "binaryoperatornode.h"
#include "logic_t.h"
#include "qbfequichecker_common.h"
#include <iosfwd>
#include <string>

namespace QBFEquiChecker_NS
{

/**
 * @class OperatorOrNode
 *
 * Operator OR / Disjunctive node
 */
class OperatorOrNode : public BinaryOperatorNode
{
    public:
        OperatorOrNode(FormulaNodeFactory& formulaNodeFactory,
                       FormulaNode& operand1,
                       FormulaNode& operand2,
                       const Index_t& index = 0,
                       const std::string& label = "");
        
		virtual ~OperatorOrNode();

        OperatorOrNode(const OperatorOrNode& rhs);
        OperatorOrNode& operator=(const OperatorOrNode& rhs);

		virtual OperatorOrNode *clone() const;

		void swap(OperatorOrNode& rhs);


    private:
        virtual Logic_t evaluate_();
        virtual void print_(std::ostream& outStrm) const;
        virtual Node_Type getNodeType_() const;
        virtual FormulaNode& simplify_();
        virtual FormulaNode& convertCNF_(FormulaNode& cnfNode);
        virtual FormulaNode& convertISCAS_(std::string& outStr);
};


} // namespace QBFEquiChecker_NS


namespace std
{
    template<>
    void swap<QBFEquiChecker_NS::OperatorOrNode>(
              QBFEquiChecker_NS::OperatorOrNode& lhs,
              QBFEquiChecker_NS::OperatorOrNode& rhs);
}


#endif // OPERATORORNODE_H

