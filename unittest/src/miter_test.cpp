#include "miter.h"
#include "unittest_helper.h"
#include <tut/tut.hpp>

#include <iostream>
#include <string>
#include <map>


using namespace QBFEquiChecker_NS;


namespace
{
    struct Miter_Test
    {
        Miter_Test()
        {
            std::clog << "Miter_Test starting...\n";
        }

        ~Miter_Test()
        {
            std::clog << "Miter_Test closing...\n";
           
        }
    };
}


namespace
{
    tut::test_group<Miter_Test>
        testGroup("Miter_Test");
}


namespace tut
{
    typedef tut::test_group<Miter_Test>::object TestObject;


	/**
     * Test 1: 
     * Test
     */
    template<>
    template<>
    void TestObject::test<1>()
    {
        std::clog << "\n[START OF TEST 1]\n";

        std::string outwire1 = "abc";
        std::string outwire2 = "xyz";
        std::string formula1 = "a | b & a";
        std::string formula2 = "(x & y) | y";

        Miter miter;
        miter.addFormula(outwire1, formula1);
        miter.addFormula(outwire2, formula2);
        miter.connectInputWires("a", "x");
        miter.connectInputWires("y", "b");

        std::map<std::string, std::string> my_map = miter.getMap();

        std::clog << "abc -> " << my_map["abc"] << std::endl;
        std::clog << "xyz -> " << my_map["xyz"] << std::endl;

        UT_ASSERT(my_map["abc"] == "x | b & x");
        UT_ASSERT(my_map["xyz"] == "(x & b) | b");

        std::clog << "\n[END OF TEST 1]\n";
    } // test 1


    /**
     * Test 2: Test addOutputWire
     */
    template<>
    template<>
    void TestObject::test<2>()
    {
        std::clog << "\n[START OF TEST 2]\n";

        std::string outwire1 = "abc";
        std::string outwire2 = "xyz";
        std::string formula1 = "a | b";
        std::string formula2 = "(x & y)";

        Miter miter;
        miter.addFormula(outwire1, formula1);
        miter.addFormula(outwire2, formula2);

        std::map<std::string, std::string> my_map = miter.getMap();
        std::clog << "abc -> " << my_map["abc"] << std::endl;
        std::clog << "xyz -> " << my_map["xyz"] << std::endl;

        UT_ASSERT(my_map["abc"] == "a | b");
        UT_ASSERT(my_map["xyz"] == "(x & y)");

        miter.connectOutputWires("abc", "xyz");

//        std::clog << "miter formula -> " << miter.generateMiterFormula() << std::endl;
//        UT_ASSERT(miter.generateMiterFormula() == "(~((a | b) ^ ((x & y))))");

        std::clog << "miter formula -> " << miter.generateMiterFormula() << std::endl;
        UT_ASSERT(miter.generateMiterFormula() == "(~((a | b) ^ ((x & y))))");


        std::clog << "\n[END OF TEST 2]\n";
    } // test 2


    /**
     * Test 3: Test generateMiterFormula
     */
    template<>
    template<>
    void TestObject::test<3>()
    {
        std::clog << "\n[START OF TEST 3]\n";

        std::string outwire1 = "abc";
        std::string outwire2 = "xyz";
        std::string outwire3 = "whatever";
        std::string formula1 = "a | b";
        std::string formula2 = "(x & y)";
        std::string formula3 = "~ew | o";

        Miter miter;
        miter.addFormula(outwire1, formula1);
        miter.addFormula(outwire2, formula2);
        miter.addFormula(outwire3, formula3);
        miter.connectOutputWires("xyz", "abc");

        std::map<std::string, std::string> my_map = miter.getMap();

        std::clog << "abc -> " << my_map["abc"] << std::endl;
        std::clog << "xyz -> " << my_map["xyz"] << std::endl;

        std::clog << "miter formula: " << miter.generateMiterFormula() << std::endl;
        UT_ASSERT("(~(((x & y)) ^ (a | b))) & (((~(whatever)) | (~ew | o)) & ((whatever) | (~(~ew | o))))" == miter.generateMiterFormula());

        std::clog << "\n[END OF TEST 3]\n";
    } // test 3


    /**
     * Test 4: Test generateMiterFormula (replacing wires)
     */
    template<>
    template<>
    void TestObject::test<4>()
    {
        std::clog << "\n[START OF TEST 4]\n";

        std::string outwire1 = "a";
        std::string outwire2 = "aa";
        std::string outwire3 = "aaa";
        std::string outwire4 = "whatever";
        std::string outwire5 = "whatever2";

        std::string formula1 = "y | x";
        std::string formula2 = "(a & y)";
        std::string formula3 = "aa | x";
        std::string formula4 = "x | aa & a | ~aaa";
        std::string formula5 = "aaa & aa";

        Miter miter;
        UT_ASSERT(miter.addFormula(outwire1, formula1));
        UT_ASSERT(miter.addFormula(outwire2, formula2));
        UT_ASSERT(miter.addFormula(outwire3, formula3));
        UT_ASSERT(miter.addFormula(outwire4, formula4));
        UT_ASSERT(miter.addFormula(outwire5, formula5));

        UT_ASSERT(miter.connectOutputWires("whatever", "whatever2"));

        std::clog << "miter formula: " << miter.generateMiterFormula() << std::endl;
        UT_ASSERT("(~((x | aa & a | ~aaa) ^ (aaa & aa))) & (((~(a)) | (y | x)) & ((a) | (~(y | x)))) & (((~(aa)) | ((a & y))) & ((aa) | (~((a & y))))) & (((~(aaa)) | (aa | x)) & ((aaa) | (~(aa | x))))"
                    == miter.generateMiterFormula());

        std::clog << "\n[END OF TEST 4]\n";
    } // test 4

}

