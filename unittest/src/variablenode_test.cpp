#include "variablenode.h"
#include "logic_t.h"
#include "formulaparserexception.h"
#include "formulanodefactory.h"
#include "qbfequichecker_common.h"

#include "unittest_helper.h"
#include <tut/tut.hpp>

#include <iostream>
#include <string>

using namespace QBFEquiChecker_NS;


namespace
{
    struct VariableNode_Test
    {
        VariableNode_Test()
        {
            std::clog << "VariableNode_Test starting...\n";
        }

        ~VariableNode_Test()
        {
            std::clog << "VariableNode_Test closing...\n";
           
        }

        FormulaNodeFactory factory_;
    };

}


namespace
{
    tut::test_group<VariableNode_Test> 
        testGroup("VariableNode_Test");
}


namespace tut
{
    typedef tut::test_group<VariableNode_Test>::object TestObject;


	/**
     * Test 1: 
     * Test for memory leaks
     */
    template<>
    template<>
    void TestObject::test<1>()
    {
        std::clog << "\n[START OF TEST 1]\n";

        VariableNode *data1 = new VariableNode(factory_, "whatever", true, 0);
        delete data1;

        std::clog << "\n[END OF TEST 1]\n";
    } // test 1


	/**
     * Test 2: getIndex(), getTreeHeight(), isNegated(), getVarLabel()
     */
    template<>
    template<>
    void TestObject::test<2>()
    {
        std::clog << "\n[START OF TEST 2]\n";
       
        VariableNode node(factory_, "whatever", false, 23, true);
        VariableNode node2(factory_, "again", true, 76, false);

        UT_ASSERT(node.getIndex() == 23);
        UT_ASSERT(node.getTreeHeight() == 1);
        UT_ASSERT(node.isNegated() == true);
        UT_ASSERT(node.getLabel() == "whatever");

        UT_ASSERT(node2.getIndex() == 76);
        UT_ASSERT(node2.getTreeHeight() == 1);
        UT_ASSERT(node2.isNegated() == false);
        UT_ASSERT(node2.getLabel() == "again");

        std::clog << "\n[END OF TEST 2]\n";
    } // test 2


	/**
     * Test 3:  copy contructor and assignment
     */
    template<>
    template<>
    void TestObject::test<3>()
    {
        std::clog << "\n[START OF TEST 3]\n";
 
        VariableNode node(factory_, "hello", false, 1299, true);
        VariableNode node2(node);
        VariableNode node3(factory_, "a", true, 22, false);
        node3 = node2;
 
        UT_ASSERT(node.getIndex() == node2.getIndex());
        UT_ASSERT(node3.getIndex() == node2.getIndex());
        UT_ASSERT(node2.getTreeHeight() == node.getTreeHeight());
        UT_ASSERT(node3.getTreeHeight() == node.getTreeHeight());
        UT_ASSERT(node2.isNegated() == node3.isNegated());
        UT_ASSERT(node.isNegated() == node3.isNegated());
        UT_ASSERT(node2.getLabel() == node.getLabel());
        UT_ASSERT(node2.getLabel() == node3.getLabel());

        std::clog << "\n[END OF TEST 3]\n";
    } // test 3


	/**
     * Test 4:  swap()
     */
    template<>
    template<>
    void TestObject::test<4>()
    {
        std::clog << "\n[START OF TEST 4]\n";
 
        VariableNode node(factory_, "hello", false, 1299, true);
        VariableNode node2(factory_, "abc", true, 0, false);

        node2.swap(node);
 
        UT_ASSERT(node.getIndex() == 0);
        UT_ASSERT(node2.getIndex() == 1299);
        UT_ASSERT(node2.getTreeHeight() == node.getTreeHeight());
        UT_ASSERT(node2.isNegated() == true);
        UT_ASSERT(node.isNegated() == false);
        UT_ASSERT(node2.getLabel() == "hello");
        UT_ASSERT(node.getLabel() == "abc");

        std::clog << "\n[END OF TEST 4]\n";
    } // test 4


	/**
     * Test 5: print()
     */
    template<>
    template<>
    void TestObject::test<5>()
    {
        std::clog << "\n[START OF TEST 5]\n";

        VariableNode node(factory_, "hello", false, 1299, true);
        VariableNode node2(factory_, "llo", true, 701, false);
        VariableNode node3(factory_, "hello", false, 0, true);
        VariableNode node4(factory_, "llo", true, 0, false);
       
        std::clog << "node: " << node << std::endl;
        std::clog << "node2: " << node2 << std::endl; 
        std::clog << "node3: " << node3 << std::endl;
        std::clog << "node4: " << node4 << std::endl;
 
        std::clog << "\n[END OF TEST 5]\n";
    } // test 5


	/**
     * Test 6: evaluate()
     */
    template<>
    template<>
    void TestObject::test<6>()
    {
        std::clog << "\n[START OF TEST 6]\n";

        VariableNode node(factory_, "hello", false, 1299, true);
        VariableNode node2(factory_, "llo", true, 701, false);
        VariableNode node3(factory_, "hello", Logic_t::FALSE, 0, false);
        VariableNode node4(factory_, "llo", Logic_t::TRUE, 0, true);
        VariableNode node5(factory_, "hello", Logic_t::SYMBOLIC, 0, false);
        VariableNode node6(factory_, "llo", Logic_t::SYMBOLIC, 0, true);
        
        UT_ASSERT(node.evaluate() == true);
        UT_ASSERT(node2.evaluate() == true);
        UT_ASSERT(node3.evaluate() == false);
        UT_ASSERT(node4.evaluate() == false);
        UT_ASSERT(node5.evaluate() == Logic_t::SYMBOLIC);
        UT_ASSERT(node6.evaluate() == Logic_t::SYMBOLIC);
       
        std::clog << "\n[END OF TEST 6]\n";
    } // test 6


	/**
     * Test 7: setVariableValue()
     */
    template<>
    template<>
    void TestObject::test<7>()
    {
        std::clog << "\n[START OF TEST 7]\n";

        VariableNode node(factory_, "hello", false, 1299, true);
        
        node.setVariableValue(1299, Logic_t::SYMBOLIC); 
        UT_ASSERT(node.evaluate() == Logic_t::SYMBOLIC);
        node.setVariableValue(1299, true); 
        UT_ASSERT(node.evaluate() == false);
        node.setVariableValue(1299, Logic_t(Logic_t::FALSE)); 
        UT_ASSERT(node.evaluate() == true);
       
        std::clog << "\n[END OF TEST 7]\n";
    } // test 6


}

