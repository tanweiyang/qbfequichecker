#include "operatorandnode.h"
#include "operatornotnode.h"
#include "cnfnode.h"
#include "variablenode.h"
#include "logic_t.h"
#include "formulaparserexception.h"
#include "qbfequichecker_common.h"
#include "formulanodefactory.h"

#include "unittest_helper.h"
#include <tut/tut.hpp>

#include <iostream>
#include <string>
#include <sstream>


using namespace QBFEquiChecker_NS;


namespace
{
    struct OperatorAndNode_Test
    {
        OperatorAndNode_Test()
        {
            std::clog << "OperatorAndNode_Test starting...\n";
        }

        ~OperatorAndNode_Test()
        {
            std::clog << "OperatorAndNode_Test closing...\n";
           
        }

        FormulaNodeFactory factory_;
    };

}


namespace
{
    tut::test_group<OperatorAndNode_Test> 
        testGroup("OperatorAndNode_Test");
}


namespace tut
{
    typedef tut::test_group<OperatorAndNode_Test>::object TestObject;


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
        VariableNode node2(factory_, "again", true, 76, false);

        OperatorAndNode *data1 = new OperatorAndNode(factory_, node, node2);
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
        VariableNode node2(factory_, "again", true, 76, false);
        OperatorAndNode node3(factory_, node, node2);
        OperatorAndNode node4(factory_, node, node3);

        UT_ASSERT(node2.getTreeHeight() == 1);
        UT_ASSERT(node3.getTreeHeight() == 2);

        std::clog << "\n[END OF TEST 2]\n";
    } // test 2


	/**
     * Test 3:  setVariableValue() and evaluate()
     */
    template<>
    template<>
    void TestObject::test<3>()
    {
        std::clog << "\n[START OF TEST 3]\n";
 
        VariableNode node(factory_, "whatever", true, 23, true);
        VariableNode node2(factory_, "again", true, 76, false);
        OperatorAndNode node3(factory_, node2, node2);
        OperatorAndNode node4(factory_, node, node3);

        UT_ASSERT(node4.evaluate() == false);

        node4.setVariableValue(23, Logic_t::FALSE);
        UT_ASSERT(node4.evaluate() == true);

        node4.setVariableValue(23, Logic_t::SYMBOLIC);
        UT_ASSERT(node4.evaluate() == Logic_t::SYMBOLIC);

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
        VariableNode node2(factory_, "again", true, 76, false);
        OperatorAndNode node3(factory_, node, node2);
        OperatorAndNode node4(factory_, node, node3);

        OperatorAndNode node5(node4);
        UT_ASSERT(node5.evaluate() == false);

        OperatorAndNode node6(node5);

        node4.setVariableValue(23, Logic_t::FALSE);
        node6 = node4;
        UT_ASSERT(node6.evaluate() == true);

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
  
        VariableNode node(factory_, "whatever", true, 23, false);
        VariableNode node1(factory_, "again", true, 16, true);
        VariableNode node2(factory_, "again", true, 76, true);
        OperatorAndNode node3(factory_, node, node2);
        OperatorAndNode node4(factory_, node, node3);

        OperatorAndNode node5(factory_, node1, node1);
        OperatorAndNode node6(node5);
        node4.setVariableValue(76, Logic_t::FALSE);

        node6.swap(node4);

        UT_ASSERT(node4.evaluate() == false);
        UT_ASSERT(node6.evaluate() == true);

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
        VariableNode node2(factory_, "again", true, 76, false);
        OperatorAndNode node3(factory_, node, node2);

        std::clog << node3 << std::endl;

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
        VariableNode node2(factory_, "b", true, 76, false);
        OperatorNotNode notNode(factory_, node);
        OperatorAndNode node3(factory_, notNode, node2);

        CNFNode cnfNode(factory_);

        node3.convertCNF(cnfNode);
        cnfNode.print(ss);
        std::clog << "After convertCNF: " << ss.str() << std::endl;
        UT_ASSERT(ss.str() == "((a | (~b) | @w0) & ((~a) | (~@w0)) & (b | (~@w0)))");

        std::clog << node3 << std::endl;

        std::clog << "\n[END OF TEST 7]\n";
    } // test 7

}

