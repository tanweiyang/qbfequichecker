/**
 * @file formulanodefactory.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef FORMULANODEFACTORY_H
#define FORMULANODEFACTORY_H


#include "logic_t.h"
#include "qbfequichecker_common.h"
#include "uncopyable.h"
#include <string>
#include <vector>


namespace QBFEquiChecker_NS
{

class FormulaNode;


/**
 * @class FormulaNodeFactory
 *
 * Creates and destroys nodes. Basically maintain the nodes and their indices
 */
class FormulaNodeFactory : private Uncopyable
{
    public:
  
        /**
         * Difference between createVariableNode and createLiteralNode: createVariableNode will do an additional check of whether 
         * the variable exists based on the label string. If it does exist, it will not create a new node 
         */
        FormulaNode& createVariableNode(const std::string& label = "",
                                        const Logic_t::LOGIC_TYPE_E_& value = Logic_t::SYMBOLIC,
                                        bool isNegated = false);
        FormulaNode& createLiteralNode(const std::string& label = "",
                                       const Logic_t::LOGIC_TYPE_E_& value = Logic_t::SYMBOLIC,
                                       bool isNegated = false);
 

        /**
         * Difference between createVariableNode and createLiteralNode: createVariableNode will do an additional check of whether 
         * the variable exists based on the label string. If it does exist, it will not create a new node 
         */
        FormulaNode& createVariableNode(const std::string& label,
                                        bool value,
                                        bool isNegated = false);
        FormulaNode& createLiteralNode(const std::string& label,
                                       bool value,
                                       bool isNegated = false);
  
        FormulaNode& createTrueNode();
        FormulaNode& createFalseNode();


        FormulaNode& createOperatorAndNode(FormulaNode& operand1,
                                           FormulaNode& operand2,
                                           const std::string& label = "");
        FormulaNode& createOperatorOrNode(FormulaNode& operand1,
                                          FormulaNode& operand2,
                                          const std::string& label = "");
        FormulaNode& createOperatorXorNode(FormulaNode& operand1,
                                           FormulaNode& operand2,
                                           const std::string& label = "");
        FormulaNode& createOperatorNotNode(FormulaNode& operand,
                                           const std::string& label = "");
 
        FormulaNode& createClauseNode(const std::vector<FormulaNode *>& literalVector,
                                      const std::string& label = "");
        FormulaNode& createClauseNode(const std::string& label = "");
        FormulaNode& createCNFNode(const std::vector<FormulaNode *>& clauseVector,
                                   const std::string& label = "");
        FormulaNode& createCNFNode(const std::string& label = "");
       

        /**
         * This function only decrement the internal counter, as too dangerous to free the heap memory.
         * Use void freeMemory() to free memory after decrementing the internal counter
         */
        bool removeFormulaNode(FormulaNode& node);


        /**
         * Decrement the counter recursively
         * @param node
         * @return
         */
//        void removeFormulaNodeRecur(FormulaNode& node);


        /**
         * Delete pointers that have corresponding zero counter
         * Indices of the nodes will the altered too
         */
        void freeMemory();


        /**
         * @return number of variables
         */
        Size_Type getNumOfVar() const;
        

        /// for debugging (includes true / false nodes)
        unsigned int totalNumOfNodes() const;


        /**
         * Clear and delete all the nodes
         */
        void clear();


        /**
         * @param index - node's index
         * @return the node label
         */
        std::string getVarName(const Index_t& index) const;


        /**
         * @param varLabel - the node's label
         * @return the index of the node
         */
        Index_t getVarIndex(const std::string& varLabel) const;


        FormulaNodeFactory();
        ~FormulaNodeFactory();


    private:
        struct PImpl;
        PImpl *pImpl_;
};


} // namespace QBFEquiChecker_NS



#endif // FORMULANODEFACTORY_H

