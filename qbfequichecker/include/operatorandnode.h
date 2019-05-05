/**
 * @file operatorandnode.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef OPERATANDANDNODE_H
#define OPERATANDANDNODE_H


#include "binaryoperatornode.h"
#include "logic_t.h"
#include "qbfequichecker_common.h"
#include <iosfwd>
#include <string>


namespace QBFEquiChecker_NS
{

class FormulaNodeFactory;

/**
 * @class OperatorAndNode
 *
 * Operator AND node
 */
class OperatorAndNode : public BinaryOperatorNode
{
    public:
        OperatorAndNode(FormulaNodeFactory& formulaNodeFactory,
                        FormulaNode& operand1,
                        FormulaNode& operand2,
                        const Index_t& index = 0,
                        const std::string& label = "");
        
		virtual ~OperatorAndNode();

        OperatorAndNode(const OperatorAndNode& rhs);
        OperatorAndNode& operator=(const OperatorAndNode& rhs);

		virtual OperatorAndNode *clone() const;

		void swap(OperatorAndNode& rhs);


    private:
        virtual Logic_t evaluate_();
        virtual void print_(std::ostream& outStrm) const;
        virtual Node_Type getNodeType_() const;
        virtual FormulaNode& simplify_();
        virtual FormulaNode& convertCNF_(FormulaNode& cnfNode);
        virtual FormulaNode& convertISCAS_(std::string& outStr) ;
};


} // namespace QBFEquiChecker_NS


namespace std
{
    template<>
    void swap<QBFEquiChecker_NS::OperatorAndNode>(
              QBFEquiChecker_NS::OperatorAndNode& lhs,
              QBFEquiChecker_NS::OperatorAndNode& rhs);
}


#endif // OPERATANDANDNODE_H

