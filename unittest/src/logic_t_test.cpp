#include "logic_t.h"
#include "unittest_helper.h"
#include <tut/tut.hpp>

#include <iostream>
#include <list>


using namespace QBFEquiChecker_NS;

namespace
{
    struct Logic_t_Test
    {
        Logic_t_Test()
        {
            std::clog << "Logic_t_Test starting...\n";
        }

        ~Logic_t_Test()
        {
            std::clog << "Logic_t_Test closing...\n";
        }
    };

}


namespace
{
    tut::test_group<Logic_t_Test> testGroup("Logic_t_Test");
}


namespace tut
{
    typedef tut::test_group<Logic_t_Test>::object TestObject;


    /**
     * Test == and !=
     */
    template<>
    template<>
    void TestObject::test<1>()
    {
        std::clog << "\n[START OF TEST 1]\n";

        Logic_t l1(true);
        Logic_t l2(false);
        Logic_t l3(Logic_t::TRUE);
        Logic_t l4(Logic_t::FALSE);
        Logic_t l5(Logic_t::SYMBOLIC);
        Logic_t l6(Logic_t::SYMBOLIC);

        UT_ASSERT(l1 == true);
        UT_ASSERT(l1 != false);
        UT_ASSERT(l3 == true);
        UT_ASSERT(l3 != false);
        UT_ASSERT(l2 == false);
        UT_ASSERT(l2 != true);
        UT_ASSERT(l4 == false);
        UT_ASSERT(l4 != true);
        UT_ASSERT(l5 != false);
        UT_ASSERT(l5 != true);

        UT_ASSERT(l1 == Logic_t::TRUE);
        UT_ASSERT(l1 != Logic_t::FALSE);
        UT_ASSERT(l3 == Logic_t::TRUE);
        UT_ASSERT(l3 != Logic_t::FALSE);
        UT_ASSERT(l2 == Logic_t::FALSE);
        UT_ASSERT(l2 != Logic_t::TRUE);
        UT_ASSERT(l4 == Logic_t::FALSE);
        UT_ASSERT(l4 != Logic_t::TRUE);
        UT_ASSERT(l5 != Logic_t::FALSE);
        UT_ASSERT(l5 != Logic_t::TRUE);

        UT_ASSERT(l5 == l6);
        UT_ASSERT(l5 != l4);
        UT_ASSERT(l5 != l3);
        UT_ASSERT(l5 != l2);
        UT_ASSERT(l5 != l1);
        UT_ASSERT(l4 != l3);
        UT_ASSERT(l4 == l2);
        UT_ASSERT(l4 != l1);
        UT_ASSERT(l3 != l2);
        UT_ASSERT(l3 == l1);
        UT_ASSERT(l2 != l1);
        UT_ASSERT(l2 == l2);

 
        std::clog << "\n[END OF TEST 1]\n";
    } // test 1


    /**
     * Test swap() 
     */
    template<>
    template<>
    void TestObject::test<2>()
    {
        std::clog << "\n[START OF TEST 2]\n";

        Logic_t l1(true);
        Logic_t l2(false);
        Logic_t l3(Logic_t::SYMBOLIC);

        l1.swap(l2);
        UT_ASSERT(l1 == false);
        UT_ASSERT(l2 == true);

        l3.swap(l2);
        UT_ASSERT(l2 != false);
        UT_ASSERT(l2 != true);
        UT_ASSERT(l3 == true);

        std::clog << "\n[END OF TEST 2]\n";
    } // test 2


    /**
     * Test | 
     */
    template<>
    template<>
    void TestObject::test<3>()
    {
        std::clog << "\n[START OF TEST 3]\n";

        Logic_t l1(true);
        Logic_t l2(false);
        Logic_t l3(Logic_t::SYMBOLIC);

        Logic_t l1orl1 = l1 | l1;
        Logic_t l1orl2 = l1 | l2;
        Logic_t l1orl3 = l1 | l3;
        Logic_t l2orl2 = l2 | l2;
        Logic_t l2orl3 = l2 | l3;
        Logic_t l3orl3 = l3 | l3;

        UT_ASSERT(l1orl1.getValue() == Logic_t::TRUE);
        UT_ASSERT(l1orl2.getValue() == Logic_t::TRUE);
        UT_ASSERT(l1orl3.getValue() == Logic_t::TRUE);
        UT_ASSERT(l2orl2.getValue() == Logic_t::FALSE);
        UT_ASSERT(l2orl3.getValue() == Logic_t::SYMBOLIC);
        UT_ASSERT(l3orl3.getValue() == Logic_t::SYMBOLIC);
        UT_ASSERT((l1 | false) == Logic_t::TRUE);
        UT_ASSERT((l1 | true) == Logic_t::TRUE);
        UT_ASSERT((false | l2) == Logic_t::FALSE);
        UT_ASSERT((true | l2) == Logic_t::TRUE);
        UT_ASSERT((l3 | true) == Logic_t::TRUE);
        UT_ASSERT((false | l3) == Logic_t::SYMBOLIC);

        UT_ASSERT((l1 | Logic_t::TRUE) == Logic_t::TRUE);
        UT_ASSERT((l2 | Logic_t::FALSE) == Logic_t::FALSE);
        UT_ASSERT((Logic_t::SYMBOLIC | l3) == Logic_t::SYMBOLIC);
        UT_ASSERT((Logic_t::TRUE | l3) == Logic_t::TRUE);
        UT_ASSERT((Logic_t::FALSE | l3) == Logic_t::SYMBOLIC);
        
        std::clog << "\n[END OF TEST 3]\n";
    } // test 3


    /**
     * Test & 
     */
    template<>
    template<>
    void TestObject::test<4>()
    {
        std::clog << "\n[START OF TEST 4]\n";

        Logic_t l1(true);
        Logic_t l2(false);
        Logic_t l3(Logic_t::SYMBOLIC);

        Logic_t l1andl1 = l1 & l1;
        Logic_t l1andl2 = l1 & l2;
        Logic_t l1andl3 = l1 & l3;
        Logic_t l2andl2 = l2 & l2;
        Logic_t l2andl3 = l2 & l3;
        Logic_t l3andl3 = l3 & l3;

        UT_ASSERT(l1andl1.getValue() == Logic_t::TRUE);
        UT_ASSERT(l1andl2.getValue() == Logic_t::FALSE);
        UT_ASSERT(l1andl3.getValue() == Logic_t::SYMBOLIC);
        UT_ASSERT(l2andl2.getValue() == Logic_t::FALSE);
        UT_ASSERT(l2andl3.getValue() == Logic_t::FALSE);
        UT_ASSERT(l3andl3.getValue() == Logic_t::SYMBOLIC);

        UT_ASSERT((l1 & false) == Logic_t::FALSE);
        UT_ASSERT((l1 & true) == Logic_t::TRUE);
        UT_ASSERT((false & l2) == Logic_t::FALSE);
        UT_ASSERT((true & l2) == Logic_t::FALSE);
        UT_ASSERT((l3 & true) == Logic_t::SYMBOLIC);
        UT_ASSERT((false & l3) == Logic_t::FALSE);

        UT_ASSERT((l1 & Logic_t::TRUE) == Logic_t::TRUE);
        UT_ASSERT((l2 & Logic_t::TRUE) == Logic_t::FALSE);
        UT_ASSERT((Logic_t::SYMBOLIC & l3) == Logic_t::SYMBOLIC);
        UT_ASSERT((Logic_t::TRUE & l3) == Logic_t::SYMBOLIC);
        UT_ASSERT((Logic_t::FALSE & l3) == Logic_t::FALSE);
 
        std::clog << "\n[END OF TEST 4]\n";
    } // test 4


    /**
     * Test ~ 
     */
    template<>
    template<>
    void TestObject::test<5>()
    {
        std::clog << "\n[START OF TEST 5]\n";

        Logic_t l1(true);
        Logic_t l2(false);
        Logic_t l3(Logic_t::SYMBOLIC);

        Logic_t notl1 = ~l1;
        Logic_t notl2 = ~l2;
        Logic_t notl3 = ~l3;

        UT_ASSERT(notl1.getValue() == Logic_t::FALSE);
        UT_ASSERT(notl2.getValue() == Logic_t::TRUE);
        UT_ASSERT(notl3.getValue() == Logic_t::SYMBOLIC);

        std::clog << "\n[END OF TEST 5]\n";
    } // test 5


    /**
     * Test 6: isSymbolic()
     */
    template<>
    template<>
    void TestObject::test<6>()
    {
        std::clog << "\n[START OF TEST 6]\n";

        Logic_t l1(true);
        Logic_t l2(false);
        Logic_t l3(Logic_t::TRUE);
        Logic_t l4(Logic_t::FALSE);
        Logic_t l5(Logic_t::SYMBOLIC);

        UT_ASSERT(!l1.isSymbolic());
        UT_ASSERT(!l2.isSymbolic());
        UT_ASSERT(!l3.isSymbolic());
        UT_ASSERT(!l4.isSymbolic());
        UT_ASSERT(l5.isSymbolic());

        std::clog << "\n[END OF TEST 6]\n";
    } // test 6


    /**
     * Test 7: print()
     */
    template<>
    template<>
    void TestObject::test<7>()
    {
        std::clog << "\n[START OF TEST 7]\n";

        Logic_t l1(true);
        Logic_t l2(false);
        Logic_t l3(Logic_t::TRUE);
        Logic_t l4(Logic_t::FALSE);
        Logic_t l5(Logic_t::SYMBOLIC);

        std::clog << 
           "l1: " << l1 << 
           " l2: " << l2 <<
           " l3: " << l3 <<
           " l4: " << l4 <<
           " l5: " << l5 <<
           std::endl;

        std::clog << "\n[END OF TEST 7]\n";
    } // test 7

}

