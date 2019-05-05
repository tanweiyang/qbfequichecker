#include "formulanodefactory.h"
#include "formulanode.h"
#include "binaryoperatornode.h"
#include "unaryoperatornode.h"
#include "operatorornode.h"
#include "operatornotnode.h"
#include "operatorandnode.h"
#include "literalnode.h"
#include "variablenode.h"
#include "cnfnode.h"
#include "clausenode.h"
#include "truenode.h"
#include "falsenode.h"
#include "operatornode.h"
#include "logic_t.h"
#include "formulaparserexception.h"
#include "qbfequichecker_common.h"

#include "unittest_helper.h"
#include <tut/tut.hpp>

#include <iostream>
#include <string>


using namespace QBFEquiChecker_NS;


namespace
{
    struct FormulaNodeFactory_Test
    {
        FormulaNodeFactory_Test()
        {
            std::clog << "FormulaNodeFactory_Test starting...\n";
        }

        ~FormulaNodeFactory_Test()
        {
            std::clog << "FormulaNodeFactory_Test closing...\n";
           
        }
    };

}


namespace
{
    tut::test_group<FormulaNodeFactory_Test> 
        testGroup("FormulaNodeFactory_Test");
}


namespace tut
{
    typedef tut::test_group<FormulaNodeFactory_Test>::object TestObject;


	/**
     * Test 1: 
     * Test creation and deletions
     */
    template<>
    template<>
    void TestObject::test<1>()
    {
        std::clog << "\n[START OF TEST 1]\n";

        FormulaNodeFactory factory;

        FormulaNode& data = factory.createVariableNode("whatever", true, false);
        UT_ASSERT(data.getIndex() == 1);
        FormulaNode& data1 = factory.createVariableNode("whatever", true, false);
        UT_ASSERT(data1.getIndex() == 1);
        FormulaNode& data2 = factory.createVariableNode("whatever1", true, false);
        UT_ASSERT(data2.getIndex() == 2);
        
        UT_ASSERT(factory.removeFormulaNode(data1));
        factory.freeMemory();
        UT_ASSERT(data2.getIndex() == 2);
        UT_ASSERT(factory.removeFormulaNode(data));
        factory.freeMemory();
        UT_ASSERT(data2.getIndex() == 1);

        std::clog << "\n[END OF TEST 1]\n";
    } // test 1


	/**
     * Test 2: getIndex(), getTreeHeight(), isNegated(), getLabel()
     */
    template<>
    template<>
    void TestObject::test<2>()
    {
        std::clog << "\n[START OF TEST 2]\n";
       
        std::clog << "\n[END OF TEST 2]\n";
    } // test 2


}

