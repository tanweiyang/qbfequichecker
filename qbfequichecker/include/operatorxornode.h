/**
 * @file operatorxornode.h
 * @author Wei Yang Tan
 * @date 28 Nov 2012
 */


#ifndef OPERATORXORNODE_H
#define OPERATORXORNODE_H


#include "binaryoperatornode.h"
#include "logic_t.h"
#include "qbfequichecker_common.h"
#include <iosfwd>
#include <string>

namespace QBFEquiChecker_NS
{

/**
 * @class OperatorXorNode
 *
 * Operator OR / Disjunctive node
 */
class OperatorXorNode : public BinaryOperatorNode
{
    public:
        OperatorXorNode(FormulaNodeFactory& formulaNodeFactory,
                        FormulaNode& operand1,
                        FormulaNode& operand2,
                        const Index_t& index = 0,
                        const std::string& label = "");
        
		virtual ~OperatorXorNode();

        OperatorXorNode(const OperatorXorNode& rhs);
        OperatorXorNode& operator=(const OperatorXorNode& rhs);

		virtual OperatorXorNode *clone() const;

		void swap(OperatorXorNode& rhs);


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
    void swap<QBFEquiChecker_NS::OperatorXorNode>(
              QBFEquiChecker_NS::OperatorXorNode& lhs,
              QBFEquiChecker_NS::OperatorXorNode& rhs);
}


#endif // OPERATORXORNODE_H

