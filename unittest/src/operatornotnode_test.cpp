#include "operatornotnode.h"
#include "variablenode.h"
#include "logic_t.h"
#include "cnfnode.h"
#include "clausenode.h"
#include "formulaparserexception.h"
#include "qbfequichecker_common.h"
#include "formulanodefactory.h"

#include "unittest_helper.h"
#include <tut/tut.hpp>

#include <iostream>
#include <string>


using namespace QBFEquiChecker_NS;


namespace
{
    struct OperatorNotNode_Test
    {
        OperatorNotNode_Test()
        {
            std::clog << "OperatorNotNode_Test starting...\n";
        }

        ~OperatorNotNode_Test()
        {
            std::clog << "OperatorNotNode_Test closing...\n";
           
        }

        FormulaNodeFactory factory_;
    };

}


namespace
{
    tut::test_group<OperatorNotNode_Test> 
        testGroup("OperatorNotNode_Test");
}


namespace tut
{
    typedef tut::test_group<OperatorNotNode_Test>::object TestObject;


	/**
     * Test 1: 
     * Test for memory leaks
     */
    template<>
    template<>
    void TestObject::test<1>()
    {
        std::clog << "\n[START OF TEST 1]\n";
        
        VariableNode node(factory_, "whatever", false, 23, true);

        OperatorNotNode *data1 = new OperatorNotNode(factory_, node);
        delete data1;

        std::clog << "\n[END OF TEST 1]\n";
    } // test 1


	/**
     * Test 2: getTreeHeight())
     */
    template<>
    template<>
    void TestObject::test<2>()
    {
        std::clog << "\n[START OF TEST 2]\n";
       
        VariableNode node(factory_, "whatever", false, 23, true);
        OperatorNotNode node2(factory_, node);

        UT_ASSERT(node2.getTreeHeight() == 2);

        std::clog << "\n[END OF TEST 2]\n";
    } // test 2


	/**
     * Test 3:  setVariablueValue() and evaluate()
     */
    template<>
    template<>
    void TestObject::test<3>()
    {
        std::clog << "\n[START OF TEST 3]\n";
 
        VariableNode node(factory_, "whatever", true, 23, true);
        OperatorNotNode node2(factory_, node);

        UT_ASSERT(node2.evaluate() == true);

        node2.setVariableValue(23, Logic_t::FALSE);
        UT_ASSERT(node2.evaluate() == false);

        node2.setVariableValue(23, Logic_t::SYMBOLIC);
        UT_ASSERT(node2.evaluate() == Logic_t::SYMBOLIC);

        std::clog << "\n[END OF TEST 3]\n";
    } // test 3


	/**
     * Test 4:  copy contructor and assignment
     */
    template<>
    template<>
    void TestObject::test<4>()
    {
        std::clog << "\n[START OF TEST 4]\n";
  
        VariableNode node(factory_, "whatever", true, 23, true);
        OperatorNotNode node2(factory_, node);
        OperatorNotNode node3(node2);
        UT_ASSERT(node3.evaluate() == true);

        OperatorNotNode node4(node3);

        node3.setVariableValue(23, Logic_t::FALSE);
        node4 = node3;
        UT_ASSERT(node4.evaluate() == false);

        std::clog << "\n[END OF TEST 4]\n";
    } // test 4


	/**
     * Test 5:  swap()
     */
    template<>
    template<>
    void TestObject::test<5>()
    {
        std::clog << "\n[START OF TEST 5]\n";
  
        VariableNode node(factory_, "whatever", true, 23, true);
        VariableNode node1(factory_, "whatever", false, 53);
        OperatorNotNode node2(factory_, node);
        OperatorNotNode node3(factory_, node1);
        node3.setVariableValue(53, Logic_t::TRUE);

        node2.swap(node3);

        UT_ASSERT(node2.evaluate() == false);
        UT_ASSERT(node3.evaluate() == true);

        std::clog << "\n[END OF TEST 5]\n";
    } // test 5


	/**
     * Test 6: print()
     */
    template<>
    template<>
    void TestObject::test<6>()
    {
        std::clog << "\n[START OF TEST 6]\n";
 
        VariableNode node(factory_, "whatever", false, 23, true);
        OperatorNotNode node2(factory_, node);

        std::clog << node2 << std::endl;

        std::clog << "\n[END OF TEST 6]\n";
    } // test 6


    /**
     * Test 7: convertCNF()
     */
    template<>
    template<>
    void TestObject::test<7>()
    {
        std::clog << "\n[START OF TEST 7]\n";

        std::stringstream ss;

        VariableNode node(factory_, "a", false, 23, false);
        OperatorNotNode notNode1(factory_, node);
        OperatorNotNode notNode2(factory_, notNode1);

        CNFNode cnfNode(factory_);
        ClauseNode clauseNode(factory_);
        FormulaNode& outnode = notNode1.convertCNF(cnfNode);
        clauseNode.addChildNode(outnode);
        cnfNode.addChildNode(clauseNode);

        cnfNode.print(ss);
        std::clog << "After convertCNF: " << ss.str() << std::endl;
        UT_ASSERT(ss.str() == "(((~a)))");

        CNFNode cnfNode2(factory_);
        ss.str("");
        FormulaNode& outnode2 = notNode2.convertCNF(cnfNode);
        ClauseNode clauseNode2(factory_);
        clauseNode2.addChildNode(outnode2);
        cnfNode2.addChildNode(clauseNode2);
        cnfNode2.print(ss);
        std::clog << "After convertCNF: " << ss.str() << std::endl;
        UT_ASSERT(ss.str() == "((a))");


        std::clog << "\n[END OF TEST 7]\n";
    } // test 7

}

