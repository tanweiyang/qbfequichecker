/**
 * @file operatornotnode.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */



#ifndef OPERATORNOTNODE_H
#define OPERATORNOTNODE_H


#include "unaryoperatornode.h"
#include "logic_t.h"
#include "qbfequichecker_common.h"
#include <iosfwd>
#include <string>

namespace QBFEquiChecker_NS
{

class FormulaNodeFactory;

/**
 * @class OperatorNotNode
 *
 * Operator Not node
 */
class OperatorNotNode : public UnaryOperatorNode
{
    public:
        OperatorNotNode(FormulaNodeFactory& formulaNodeFactory,
                        FormulaNode& operand,
                        const Index_t& index = 0,
                        const std::string& label = "");
        
		virtual ~OperatorNotNode();

        OperatorNotNode(const OperatorNotNode& rhs);
        OperatorNotNode& operator=(const OperatorNotNode& rhs);

		virtual OperatorNotNode *clone() const;

		void swap(OperatorNotNode& rhs);


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
    void swap<QBFEquiChecker_NS::OperatorNotNode>(
              QBFEquiChecker_NS::OperatorNotNode& lhs,
              QBFEquiChecker_NS::OperatorNotNode& rhs);
}


#endif // OPERATORNOTNODE_H

