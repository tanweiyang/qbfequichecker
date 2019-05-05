#include "truenode.h"
#include "falsenode.h"
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
    struct TrueFalseNode_Test
    {
        TrueFalseNode_Test()
        {
            std::clog << "TrueFalseNode_Test starting...\n";
        }

        ~TrueFalseNode_Test()
        {
            std::clog << "TrueFalseNode_Test closing...\n";
           
        }

        FormulaNodeFactory factory_;
    };

}


namespace
{
    tut::test_group<TrueFalseNode_Test> 
        testGroup("TrueFalseNode_Test");
}


namespace tut
{
    typedef tut::test_group<TrueFalseNode_Test>::object TestObject;


	/**
     * Test 1: 
     * Test for memory leaks
     */
    template<>
    template<>
    void TestObject::test<1>()
    {
        std::clog << "\n[START OF TEST 1]\n";

        FormulaNode& data1 = factory_.createTrueNode();
        FormulaNode& data2 = factory_.createFalseNode();
        
        factory_.removeFormulaNode(data1);
        factory_.removeFormulaNode(data2);

        std::clog << "\n[END OF TEST 1]\n";
    } // test 1


	/**
     * Test 2: getIndex(), getTreeHeight(), isNegated()
     */
    template<>
    template<>
    void TestObject::test<2>()
    {
        std::clog << "\n[START OF TEST 2]\n";
       
        const Index_t NODE_INDEX = 0;
        const Index_t NODE2_INDEX = 45;
        FalseNode node(factory_);
        TrueNode node2(factory_, NODE2_INDEX);

        UT_ASSERT(node.getIndex() == NODE_INDEX);
        UT_ASSERT(node.getTreeHeight() == 1);
        UT_ASSERT(node.isNegated() == false);

        UT_ASSERT(node2.getIndex() == NODE2_INDEX);
        UT_ASSERT(node2.getTreeHeight() == 1);
        UT_ASSERT(node2.isNegated() == false);

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

        const Index_t INDEX1 = 1299; 
        FalseNode node(factory_, INDEX1);
        FalseNode node2(node);
        FalseNode node3(factory_);
        
        TrueNode node4(factory_);
        TrueNode node5(factory_, INDEX1);
        node5 = node4;        
        TrueNode node6(node5);
 
        UT_ASSERT(node.getIndex() == node2.getIndex());
        node = node3;
        UT_ASSERT(node3.getIndex() == node.getIndex());

        UT_ASSERT(node4.getIndex() == node5.getIndex());
        UT_ASSERT(node6.getIndex() == node5.getIndex());

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

        const Index_t INDEX1 = 675;
        const Index_t INDEX2 = 45;
 
        FalseNode node(factory_, INDEX1);
        FalseNode node2(factory_);
        TrueNode node3(factory_, INDEX2);
        TrueNode node4(factory_);

        node2.swap(node);
        node3.swap(node4);
 
        UT_ASSERT(node.getIndex() == 0);
        UT_ASSERT(node2.getIndex() == INDEX1);
        UT_ASSERT(node3.getIndex() == 0);
        UT_ASSERT(node4.getIndex() == INDEX2);

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

        TrueNode node(factory_);
        FalseNode node2(factory_);
       
        std::clog << "node: " << node << std::endl;
        std::clog << "node2: " << node2 << std::endl; 
 
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

        TrueNode node(factory_);
        FalseNode node2(factory_);
        
        UT_ASSERT(node.evaluate() == true);
        UT_ASSERT(node2.evaluate() == Logic_t::FALSE);
       
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

        TrueNode node(factory_);
        FalseNode node2(factory_, 1);
        
        UT_ASSERT(false == node.setVariableValue(0, false));
        UT_ASSERT(false == node2.setVariableValue(1, false));
       
        std::clog << "\n[END OF TEST 7]\n";
    } // test 7


}

