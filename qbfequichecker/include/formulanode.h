/**
 * @file formulanode.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef FORMULANODE_H
#define FORMULANODE_H


#include "logic_t.h"
#include "qbfequichecker_common.h"
#include <iosfwd>
#include <vector>
#include <string>

namespace QBFEquiChecker_NS
{

class FormulaNodeFactory;


/**
 * @class FormulaNode
 *
 * Abstract base class for all the node-classes
 */
class FormulaNode
{
    public:
        explicit FormulaNode(FormulaNodeFactory& formulaNodeFactory,
                             const Index_t& index,
                             const std::string& label);
        virtual ~FormulaNode() = 0;


		/**
		 * A covariant return type virtual function.
         * An idiom for duplicating the derived classes; uses copy constructor
		 */
		virtual FormulaNode *clone() const = 0;


        /**
         * @return Node type
         */
        inline Node_Type getNodeType() const
        {
            return getNodeType_();
        }


        /**
         * The main logic function: evaluate the node expression
         *
         * @throw FormulaParserException
         */
        inline Logic_t evaluate()
        {
            return evaluate_();
        }


		/**
         * Simplify the expression
         */
        inline FormulaNode& simplify()
        {
            return simplify_();
        }
        

        /**
         * Return a vector of clausesF
         */


        /**
         * Return an equivalent CNF
         */
        inline FormulaNode& convertCNF(FormulaNode& cnfNode)
        {
            return convertCNF_(cnfNode);
        }


        /**
         * Set the variable "index" with value "val"
         *
         * @return true if set successfully; false otherwise
         */
        //@{
        bool setVariableValue(Index_t index, const Logic_t& val);
        bool setVariableValue(Index_t index, const Logic_t::LOGIC_TYPE_E_& val);
        bool setVariableValue(Index_t index, bool val);
        //@}


        /**
         * Print the content of the node
         */
        inline void print(std::ostream& outStrm) const
        {
            print_(outStrm);
        }


        /**
         * Print the content of the node (in ISCAS format)
         */
        inline FormulaNode& convertISCAS(std::string& outStr)
        {
            return convertISCAS_(outStr);
        }


        inline std::string findLabel(const Index_t& index) const
        {
            return findLabel_(index);
        }


        inline Index_t findVarIndex(const std::string& varName) const
        {
            return findVarIndex_(varName);
        }


        inline std::string getLabel() const
        {
            return label_;
        }

        /**
         * Get the tree height
         *
         * @throw FormulaParserException
         */
        inline Size_Type getTreeHeight() const
        {
            return getTreeHeight_();
        }


        /**
         * @return index number of the node
         */
        inline const Index_t& getIndex() const
        {
            return index_;
        }


        /**
         * @return the index of the variable node of label
         */
        //inline const Index_t& findIndex(const std::string& label) const
        //{
        //    return findIndex_(label);
        //}


        /**
         * Set index of the node
         */
        void setIndex(const Index_t& index);


        /**
         * Add a new child node
         */
        inline bool addChildNode(FormulaNode& childNode)
        {
            return addChildNode_(childNode);
        }


        /**
          * Remove all children nodes
          */
         inline void removeAllChildrenNode()
         {
             removeAllChildrenNode_();
         }


         /**
          * Remove child node (from index)
          * @param childIndex The child node to be removed
          */
         inline bool removeChildNode(unsigned int childIndex)
         {
             return removeChildNode_(childIndex);
         }


        /**
         * To get the children nodes.
         * If the node does not have a child at childIndex,
         * null pointer is returned.
         */
        inline FormulaNode *getChild(unsigned int childIndex)
        {
            return getChild_(childIndex);
        }        
        inline const FormulaNode *getChild(unsigned int childIndex) const
        {
            return getChild_(childIndex);
        }


        /**
         * @return Number of children nodes
         */
        inline Size_Type getNumOfChildren() const
        {
            return getNumOfChildren_();
        }


        inline bool setChild(FormulaNode& childNode, unsigned int index)
        {
            return setChild_(childNode, index);
        }


//        inline bool negate()
//        {
//            return negate_();
//        }

        inline void deleteTree()
        {
            deleteTree_();
        }


	protected:
        FormulaNode(const FormulaNode& rhs);
        FormulaNode& operator=(const FormulaNode& rhs);


        /// swap
        void swap(FormulaNode& rhs);


        inline FormulaNodeFactory& getNodeFactory()
        {
            return *formulaNodeFactory_;
        }

       
    private:
        virtual Logic_t evaluate_() = 0;
        virtual void print_(std::ostream& outStrm) const = 0;
        virtual Size_Type getTreeHeight_() const = 0;
        virtual Node_Type getNodeType_() const = 0;
        virtual std::string findLabel_(const Index_t& index) const = 0;
       // virtual const Index_t& findIndex_(const std::string& label) const = 0;
        virtual Index_t findVarIndex_(const std::string& varName) const = 0;
        virtual bool setVariableVal_(const Index_t&, const Logic_t&) = 0;
        virtual FormulaNode *getChild_(unsigned int childIndex) = 0;
        virtual const FormulaNode *getChild_(unsigned int childIndex) const = 0;
        virtual Size_Type getNumOfChildren_() const = 0;
        virtual bool addChildNode_(FormulaNode& childNode) = 0;
        virtual void removeAllChildrenNode_() = 0;
        virtual bool removeChildNode_(unsigned int childIndex) = 0;
        virtual bool setChild_(FormulaNode& childNode, unsigned int index) = 0;
        virtual FormulaNode& simplify_() = 0;
        virtual void deleteTree_() = 0;
        virtual FormulaNode& convertCNF_(FormulaNode& cnfNode) = 0;
        virtual FormulaNode& convertISCAS_(std::string& outStr) = 0;
        
        FormulaNodeFactory *formulaNodeFactory_;
        Index_t index_;
        std::string label_;
};


} // namespace QBFEquiChecker_NS


std::ostream& operator<<(std::ostream& outStrm,
						 const QBFEquiChecker_NS::FormulaNode& output);


#endif // FORMULANODE_H

