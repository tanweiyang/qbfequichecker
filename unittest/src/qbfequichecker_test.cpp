/*
 * qbfequichecker_test.cpp
 *
 *  Created on: Nov 22, 2012
 *      Author: Wei Yang Tan
 */

#include "qbfequichecker.h"
#include "qbfequichecker_common.h"
#include "miter.h"
#include "quantoradaptor.h"
#include "cirqitadaptor.h"
#include "solveradaptor.h"
#include "verilogparser.h"

#include "unittest_helper.h"
#include <tut/tut.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <utility>


using namespace QBFEquiChecker_NS;


namespace
{
    struct QBFEquiChecker_Test
    {
        QBFEquiChecker_Test()
        {
            std::clog << "QBFEquiChecker_Test starting...\n";
        }

        ~QBFEquiChecker_Test()
        {
            std::clog << "QBFEquiChecker_Test closing...\n";

        }


        static void printMap(std::map<std::string, std::string>::value_type& item)
        {
            std::clog << "outVar:\"" << item.first << "\"" << std::endl
                      << "formula:\"" << item.second << "\"" << std::endl;
        }

        template <typename T>
        static void print(const T& item)
        {
            std::clog << item << " ";
        }


        struct AddFormulaToMiter
        {
            AddFormulaToMiter(Miter& miter) : miter_(miter)
            {
            }


            void operator()(std::map<std::string, std::string>::value_type& outMap)
            {
                miter_.addFormula(outMap.first, outMap.second);
            }


            Miter& miter_;
        };


        std::string trimWhiteSpace(const std::string& input)
        {
            unsigned int i = 0, j = 0;
            for(i = 0; i < input.length() && isspace(input[i]); ++i){}
            for(j = input.length(); j > 0 && isspace(input[j - 1]); --j){}

            assert(j >= i);
            return input.substr(i, j - i);
        }


        std::map<std::string, std::string> parseFile(std::string filename)
        {
            const char *KEY_WORD = "assign";
            const int KEY_LEN = 6;
            std::map<std::string, std::string> results;

            std::ifstream fileStream(filename.c_str());
            std::string outputStr;
            std::stringbuf sb(outputStr);

            fileStream >> &sb;
            outputStr = sb.str();

            size_t first_index = outputStr.find(KEY_WORD);
            while(first_index != std::string::npos)
            {
                first_index += KEY_LEN;

                size_t second_index = outputStr.find('=', first_index);
                size_t third_index = outputStr.find(';', second_index);

                if(second_index == std::string::npos ||
                   third_index == std::string::npos)
                {
                    std::cerr << "Error in the sample file" << std::endl;
                    assert(0);
                }

                std::string outVar = trimWhiteSpace(
                    outputStr.substr(first_index, second_index - first_index));

                std::string formula = trimWhiteSpace(
                     outputStr.substr(second_index + 1, (third_index - second_index) - 1));

                results.insert(std::make_pair(outVar, formula));

                outputStr = outputStr.erase(0, third_index);

                first_index = outputStr.find(KEY_WORD);
            }

            fileStream.close();

            return results;
        }


        QBFEquiChecker checker_;
    };
}


namespace
{
    tut::test_group<QBFEquiChecker_Test>
        testGroup("QBFEquiChecker_Test");
}


namespace tut
{
    typedef tut::test_group<QBFEquiChecker_Test>::object TestObject;


    /**
     * Test 1:
     * Test for some parsing of real file
     */
    template<>
    template<>
    void TestObject::test<1>()
    {
        std::clog << "\n[START OF TEST 1]\n";

        std::string f1 = " ";
        std::string f2 = "() ";
        std::string f3 = "true";
        std::string f4 = "false";
        std::string f5 = "0";
        std::string f6 = "1";
        std::string f7 = "(true)";
        std::string f8 = "(false)";
        std::string f9 = "(a[21])";
        std::string f10 = "w[0] & b";
        std::string f11 = "c_s# | W";
        std::string f12 = "((~not & ab) )| ~(zx` & ss)";
        std::string f13 = "~a & ~b | ~(c)";

        std::stringstream ss;

        UT_ASSERT(checker_.parse(f1));
        checker_.printFormula(ss);
        UT_ASSERT(QBFEquiChecker::NIL_FORMULA == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f2));
        checker_.printFormula(ss);
        UT_ASSERT(QBFEquiChecker::NIL_FORMULA == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f3));
        checker_.printFormula(ss);
        UT_ASSERT("1" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f4));
        checker_.printFormula(ss);
        UT_ASSERT(ss.str() == "0");
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f5));
        checker_.printFormula(ss);
        UT_ASSERT(ss.str() == "0");
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f6));
        checker_.printFormula(ss);
        UT_ASSERT(ss.str() == "1");
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f7));
        checker_.printFormula(ss);
        UT_ASSERT(ss.str() == "1");
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f8));
        checker_.printFormula(ss);
        UT_ASSERT(ss.str() == "0");
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f9));
        checker_.printFormula(ss);
        UT_ASSERT(ss.str() == "a[21]");
        UT_ASSERT(checker_.totalNumOfNodes() == 3);
        ss.str("");

        UT_ASSERT(checker_.parse(f10));
        checker_.printFormula(ss);
        UT_ASSERT(ss.str() == "(w[0] & b)");
        UT_ASSERT(checker_.totalNumOfNodes() == 5);
        ss.str("");

        UT_ASSERT(checker_.parse(f11));
        checker_.printFormula(ss);
        UT_ASSERT(ss.str() == "(c_s# | W)");
        UT_ASSERT(checker_.totalNumOfNodes() == 5);
        ss.str("");

        UT_ASSERT(checker_.parse(f12));
        checker_.printFormula(ss);
        std::clog << "formula: " << ss.str() << std::endl;
        UT_ASSERT(ss.str() == "(((~not) & ab) | (~(zx` & ss)))");
        UT_ASSERT(checker_.totalNumOfNodes() == 11);
        ss.str("");

        UT_ASSERT(checker_.parse(f13));
        std::clog << "\"" << f13 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        checker_.printFormula(ss);
        UT_ASSERT(ss.str() == "(((~a) & (~b)) | (~c))");
        UT_ASSERT(checker_.totalNumOfNodes() == 10);


        std::clog << "\n[END OF TEST 1]\n";
    } // test 1


    /**
     * Test 2:
     * Test for erronaeous kinds of formula string
     */
    template<>
    template<>
    void TestObject::test<2>()
    {
        std::clog << "\n[START OF TEST 2]\n";

        std::string f1 = "(";
        std::string f2 = ")";
        std::string f3 = "(true";
        std::string f4 = "false)";
        std::string f5 = "~";
        std::string wire_symbol_str = " ";
        wire_symbol_str[0] = WIRE_SYM;
        std::string f6 = wire_symbol_str + std::string(" | w");
        std::string f7 = "(~not & ab) | ";
        std::string f8 = "& ~(c)";
        std::string f9 = "c 1";

        UT_ASSERT(!checker_.parse(f1));
        UT_ASSERT(!checker_.parse(f2));
        UT_ASSERT(!checker_.parse(f3));
        UT_ASSERT(!checker_.parse(f4));
        UT_ASSERT(!checker_.parse(f5));
        UT_ASSERT(!checker_.parse(f6));
        UT_ASSERT(!checker_.parse(f7));
        UT_ASSERT(!checker_.parse(f8));
        UT_ASSERT(!checker_.parse(f9));

        std::clog << "\n[END OF TEST 2]\n";
    } // test 2


    /**
     * Test 3:
     * Test simplify
     */
    template<>
    template<>
    void TestObject::test<3>()
    {
        std::clog << "\n[START OF TEST 3]\n";

        std::string f1 = "";
        std::string f2 = "()";
        std::string f3 = "true";
        std::string f4 = "false";
        std::string f5 = "~1";
        std::string f6 = "~0";
        std::string f7 = "~~x";
        std::string f8 = "x | 1";
        std::string f9 = "y & 0";
        std::string f10 = "y | ~1";
        std::string f11 = "x & ~0";
        std::string f12 = "x & (1 | y)";
        std::string f13 = "~(~y & ~y) | ~x";
        std::string f14 = "y & (x | 0)";
        std::string f15 = "x & x";
        std::string f16 = "((y) | y)";
        std::string f17 = "~(y | ~y) & (~x | ~0)";
        std::string f18 = "~(~y & y) | (~x & ~1)";
        std::string f19 = "~(~a & b)";
        std::string f20 = "~(y | ~x)";
        std::string f21 = "~(~(y | ~x) | ~(~a & (~b & c)))";
        std::string f22 = "~(~x ^ ~x)";
        std::string f23 = "(~a ^ a)";
        std::string f24 = "(n ^ ~n)";
        std::string f25 = "(u ^ u)";


        std::stringstream ss;

        UT_ASSERT(checker_.parse(f1));
        checker_.simplify();
        checker_.printFormula(ss);
        UT_ASSERT(QBFEquiChecker::NIL_FORMULA == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f2));
        checker_.simplify();
        checker_.printFormula(ss);
        UT_ASSERT(QBFEquiChecker::NIL_FORMULA == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f3));
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f3 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("1" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f4));
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f4 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("0" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f5));
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f5 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("0" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f6));
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f6 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("1" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f7));
        checker_.simplify();
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f7 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("x" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 3);
        ss.str("");

        UT_ASSERT(checker_.parse(f8));
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f8 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("1" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f9));
        checker_.simplify();
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f9 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("0" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f10));
        checker_.simplify();
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f7 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("y" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 3);
        ss.str("");

        UT_ASSERT(checker_.parse(f11));
        checker_.simplify();
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f11 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("x" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 3);
        ss.str("");

        UT_ASSERT(checker_.parse(f12));
        checker_.simplify();
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f12 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("x" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 3);
        ss.str("");

        UT_ASSERT(checker_.parse(f13));
        checker_.simplify();
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f13 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("(y | (~x))" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 6);
        ss.str("");

        UT_ASSERT(checker_.parse(f14));
        checker_.simplify();
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f14 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("(y & x)" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 5);
        ss.str("");

        UT_ASSERT(checker_.parse(f15));
        checker_.simplify();
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f15 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("x" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 3);
        ss.str("");

        UT_ASSERT(checker_.parse(f16));
        checker_.simplify();
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f16 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("y" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 3);
        ss.str("");

        UT_ASSERT(checker_.parse(f17));
        checker_.simplify();
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f17 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("0" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f18));
        checker_.simplify();
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f18 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("1" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f19));
        checker_.simplify();
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f19 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("(~((~a) & b))" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 7);
        ss.str("");

        UT_ASSERT(checker_.parse(f20));
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f20 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("(~(y | (~x)))" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 7);
        ss.str("");

        UT_ASSERT(checker_.parse(f21));
        checker_.simplify();
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f21 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("(~((~(y | (~x))) | (~((~a) & ((~b) & c)))))" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 17);
        ss.str("");

        UT_ASSERT(checker_.parse(f22));
        checker_.simplify();
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f22 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("1" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f23));
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f23 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("1" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f24));
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f24 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("1" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f25));
        checker_.simplify();
        checker_.printFormula(ss);
        std::clog << "simplify(): \"" << f25 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("0" == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        std::clog << "\n[END OF TEST 4]\n";
    } // test 4


    /**
     * Test 5:
     * Test convertCNF
     */
    template<>
    template<>
    void TestObject::test<5>()
    {
        std::clog << "\n[START OF TEST 5]\n";

        std::string f1 = "";
        std::string f2 = "()";
        std::string f3 = "true";
        std::string f4 = "false";
        std::string f5 = "~1";
        std::string f6 = "~0";
        std::string f7 = "~~x";
        std::string f8 = "x | 1";
        std::string f9 = "y & 0";
        std::string f10 = "y | ~1";
        std::string f11 = "x & ~0";
        std::string f12 = "x & (1 | y)";
        std::string f13 = "~(~y & ~y) | ~x";
        std::string f14 = "y & (x | 0)";
        std::string f15 = "x & x";
        std::string f16 = "((y) | y)";
        std::string f17 = "~(y | ~y) & (~x | ~0)";
        std::string f18 = "~(~y & y) | (~x & ~1)";
        std::string f19 = "~(~a & b)";
        std::string f20 = "~(y | ~x)";
        std::string f21 = "~(~(y | ~x) | ~(~a & (~b & c)))";
        std::string f22 = "y & ~x | ~a";
        std::string f23 = "(a & b) | ((c | d) & e) ";
        std::string f24 = "(y & ~x) | (~a & ~c)";
        std::string f25 = "(~y | x) & (a | ~c)";
        std::string f26 = "(~y | x) & (a & ~c)";
        std::string f27 = "(~y & x) | (a | ~c)";
        std::string f28 = "((~~y & ~x) | (~a | (b & ~c)))";
        std::string f29 = "x ^ (y ^ a)";

        std::stringstream ss;
        unsigned int num_tmp;

        UT_ASSERT(checker_.parse(f1));
        checker_.convertCNF();
        checker_.printFormula(ss);
        UT_ASSERT(QBFEquiChecker::NIL_FORMULA == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f2));
        checker_.simplify();
        checker_.convertCNF();
        checker_.printFormula(ss);
        UT_ASSERT(QBFEquiChecker::NIL_FORMULA == ss.str());
        UT_ASSERT(checker_.totalNumOfNodes() == 2);
        ss.str("");

        UT_ASSERT(checker_.parse(f3));
        checker_.convertCNF();
        checker_.simplify();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << ss.str() << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("((1))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 3);
        ss.str("");

        UT_ASSERT(checker_.parse(f4));
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f4 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        UT_ASSERT("((0))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 3);
        ss.str("");

        UT_ASSERT(checker_.parse(f5));
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f5 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT("((0))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 3);
        ss.str("");

        UT_ASSERT(checker_.parse(f6));
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f6 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT("((1))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 3);
        ss.str("");

        UT_ASSERT(checker_.parse(f7));
        checker_.convertCNF();
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f7 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT("((x))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 4);
        ss.str("");

        UT_ASSERT(checker_.parse(f8));
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f8 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT("((1))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 3);
        ss.str("");

        UT_ASSERT(checker_.parse(f9));
        checker_.convertCNF();
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f9 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT("((0))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 3);
        ss.str("");

        UT_ASSERT(checker_.parse(f10));
        checker_.convertCNF();
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f7 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT("((y))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 4);
        ss.str("");

        UT_ASSERT(checker_.parse(f11));
        checker_.convertCNF();
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f11 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT("((x))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 4);
        ss.str("");

        UT_ASSERT(checker_.parse(f12));
        checker_.convertCNF();
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f12 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT("((x))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 4);
        ss.str("");

        UT_ASSERT(checker_.parse(f13));
        checker_.simplify();
        num_tmp = checker_.totalNumOfNodes();
        std::clog << std::endl << "num_tmp: " << num_tmp << std::endl;
        UT_ASSERT(checker_.convertCNF() == true);
        UT_ASSERT(!checker_.convertCNF());
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f13 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT("((y | (~x) | (~@w0)) & ((~y) | @w0) & (x | @w0) & (@w0))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 13);
        ss.str("");

        UT_ASSERT(checker_.parse(f14));
        num_tmp = checker_.totalNumOfNodes();
        std::clog << std::endl << "num_tmp: " << num_tmp << std::endl;
        checker_.convertCNF();
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f14 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT("(((~y) | (~x) | @w0) & (y | (~@w0)) & (x | (~@w0)) & (@w0))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 13);
        ss.str("");

        UT_ASSERT(checker_.parse(f15));
        num_tmp = checker_.totalNumOfNodes();
        std::clog << std::endl << "num_tmp: " << num_tmp << std::endl;
        checker_.convertCNF();
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f15 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT("((x))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 4);
        ss.str("");

        UT_ASSERT(checker_.parse(f16));
        num_tmp = checker_.totalNumOfNodes();
        std::clog << std::endl << "num_tmp: " << num_tmp << std::endl;
        checker_.convertCNF();
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f16 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT("((y))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 4);
        ss.str("");

        UT_ASSERT(checker_.parse(f17));
        num_tmp = checker_.totalNumOfNodes();
        std::clog << std::endl << "num_tmp: " << num_tmp << std::endl;
        checker_.convertCNF();
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f17 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT("((0))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 3);
        ss.str("");

        UT_ASSERT(checker_.parse(f18));
        num_tmp = checker_.totalNumOfNodes();
        std::clog << std::endl << "num_tmp: " << num_tmp << std::endl;
        checker_.convertCNF();
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f18 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT("((1))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 3);
        ss.str("");

        UT_ASSERT(checker_.parse(f19));
        checker_.simplify();
        num_tmp = checker_.totalNumOfNodes();
        std::clog << std::endl << "num_tmp: " << num_tmp << std::endl;
        checker_.convertCNF();
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f19 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        std::clog << "num of nodes: " << checker_.totalNumOfNodes() << std::endl;
        UT_ASSERT("((a | (~b) | @w0) & ((~a) | (~@w0)) & (b | (~@w0)) & ((~@w0)))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 13);
        ss.str("");

        UT_ASSERT(checker_.parse(f20));
        checker_.simplify();
        num_tmp = checker_.totalNumOfNodes();
        std::clog << std::endl << "num_tmp: " << num_tmp << std::endl;
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f20 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        //UT_ASSERT("(~(y | (~x)))" == ss.str());
        std::clog << "num of nodes: " << checker_.totalNumOfNodes() << std::endl;
        //UT_ASSERT(checker_.totalNumOfNodes() == 7);
        ss.str("");

        UT_ASSERT(checker_.parse(f21));
        checker_.simplify();
        num_tmp = checker_.totalNumOfNodes();
        std::clog << std::endl << "num_tmp: " << num_tmp << std::endl;
        checker_.convertCNF();
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f21 << "\"" << " -> ";
        checker_.printFormula(std::clog);
        std::clog << std::endl;
        //UT_ASSERT("(~((~(y | (~x))) | (~((~a) & ((~b) & c)))))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 17);
        ss.str("");

        UT_ASSERT(checker_.parse(f22));
        checker_.simplify();
        num_tmp = checker_.totalNumOfNodes();
        std::clog << std::endl << "num_tmp: " << num_tmp << std::endl;
        checker_.convertCNF();
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f22 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        //UT_ASSERT("(~((~(y | (~x))) | (~((~a) & ((~b) & c)))))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 17);
        ss.str("");

        UT_ASSERT(checker_.parse(f23));
        checker_.simplify();
        num_tmp = checker_.totalNumOfNodes();
        std::clog << std::endl << "num_tmp: " << num_tmp << std::endl;
        checker_.convertCNF();
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f23 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        //UT_ASSERT("(~((~(y | (~x))) | (~((~a) & ((~b) & c)))))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 17);
        ss.str("");




        UT_ASSERT(checker_.parse(f29));
        checker_.simplify();
        num_tmp = checker_.totalNumOfNodes();
        std::clog << std::endl << "num_tmp: " << num_tmp << std::endl;
        checker_.convertCNF();
        checker_.convertCNF();
        checker_.printCNF(ss);
        std::clog << "convertCNF(): \"" << f29 << "\"" << " -> ";
        checker_.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT("(((~y) | (~a) | (~@w0)) & (y | a | (~@w0)) & (y | (~a) | @w0) & ((~y) | a | @w0) & ((~x) | (~@w0) | (~@w1)) & (x | @w0 | (~@w1)) & (x | (~@w0) | @w1) & ((~x) | @w0 | @w1) & (@w1))" == ss.str());
        //UT_ASSERT(checker_.totalNumOfNodes() == 17);
        ss.str("");


        /*
        QBFEquiChecker parser(factory_);

        parser.parse(f1);
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f1 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT(factory_.totalNumOfNodes() == 0);

        parser.parse(f2);
        parser.convertCNF();
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f2 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT(factory_.totalNumOfNodes() == 0);

        parser.parse(f3);
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f3 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT(factory_.totalNumOfNodes() == 1);

        parser.parse(f4);
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f4 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT(factory_.totalNumOfNodes() == 1);

        parser.parse(f5);
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f5 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT(factory_.totalNumOfNodes() == 1);

        parser.parse(f6);
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f6 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT(factory_.totalNumOfNodes() == 1);

        parser.parse(f7);
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f7 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT(factory_.totalNumOfNodes() == 2);

        parser.parse(f8);
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f8 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;
        UT_ASSERT(factory_.totalNumOfNodes() == 1);

        parser.parse(f9);
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f9 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f10);
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f10 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f11);
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f11 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f12);
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f12 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f13);
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f13 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f14);
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f14 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f15);
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f15 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f16);
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f16 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f17);
        parser.convertCNF();
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f17 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f18);
        parser.convertCNF();
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f18 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f19);
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f19 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f20);
        parser.convertCNF();
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f20 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f21);
        parser.convertCNF();
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f21 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f22);
        parser.convertCNF();
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f22 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f23);
        parser.convertCNF();
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f23 << "\"" << " -> ";
        parser.printFormula(std::clog);
        std::clog << std::endl;
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f24);
        parser.convertCNF();
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f24 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f25);
        parser.convertCNF();
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f25 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f26);
        parser.convertCNF();
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f26 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f27);
        parser.convertCNF();
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f27 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        parser.parse(f28);
        parser.convertCNF();
        parser.convertCNF();
        std::clog << "convertCNF: \"" << f28 << "\"" << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;
        std::clog << "Number of nodes: " << factory_.totalNumOfNodes() << std::endl;
        UT_ASSERT(factory_.totalNumOfNodes() == 64);
*/
        std::clog << "\n[END OF TEST 5]\n";

    } // test 5


    /**
     * Test 6:
     * Test before parsing and all the print functions
     */
    template<>
    template<>
    void TestObject::test<6>()
    {
        std::clog << "\n[START OF TEST 6]\n";

        std::string f1 = "((a & ~b) | c) | (~b & d) ";

        QBFEquiChecker parser;

        std::clog << "Should be empty -> ";
        parser.printFormula(std::clog);
        std::clog << std::endl;

        std::clog << "Should be empty -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;

        std::clog << "Should be empty -> ";
        parser.printDIMACS(std::clog);
        std::clog << std::endl;

        parser.printIndexVarMap(std::clog);
        std::clog << std::endl;

        parser.parse(f1);
        parser.convertCNF();
        std::clog << "Formula: \"" << f1 << "\" -> ";

        parser.printFormula(std::clog);
        std::clog << std::endl;
        parser.printCNF(std::clog);
        std::clog << std::endl;
        parser.printDIMACS(std::clog);
        std::clog << std::endl;
        parser.printIndexVarMap(std::clog);
        std::clog << std::endl;

        std::clog << "\n[END OF TEST 6]\n";
    } // Test 6


    /**
     * Test 7:
     * Test before parsing and all the print functions. Satisfiable
     */
    template<>
    template<>
    void TestObject::test<7>()
    {
        std::clog << "\n[START OF TEST 7]\n";

        std::string f1 = "((a & ~b) | c & ~d)  ";

        std::stringstream ss;

        QBFEquiChecker parser;
        parser.parse(f1);

        parser.moveToInnerExtQuantifier("a");// e1
        parser.moveToInnerUniQuantifier("b");// e1 a2
        parser.moveToOuterUniQuantifier("c");// a3 e1 a2
        parser.moveToOuterExtQuantifier("d");// e4 a3 e1 a2

        UT_ASSERT(parser.convertCNF());
        parser.printDIMACS(ss);

        std::clog << ss.str() << std::endl;
        UT_ASSERT("c Generated by QBFEquiChecker\n"
                  "p cnf 7 10\n"
                  "e 4 0\n"
                  "a 3 0\n"
                  "e 1 0\n"
                  "a 2 0\n"
                  "e 5 6 7 0\n"
                  "-1 2 5 0\n"
                  "1 -5 0\n"
                  "-2 -5 0\n"
                  "5 3 -6 0\n"
                  "-5 6 0\n"
                  "-3 6 0\n"
                  "-6 4 7 0\n"
                  "6 -7 0\n"
                  "-4 -7 0\n"
                  "7 0\n" == ss.str());

        ss.str() = "";


        QBFEquiChecker parser2;
        parser2.parse(f1);

        parser2.moveToInnerExtQuantifier("a");// e1
        parser2.moveToInnerUniQuantifier("b");// e1 a2
        parser2.moveToOuterExtQuantifier("c");// a3 e1 a2
        parser2.moveToOuterExtQuantifier("d");// e4 a3 e1 a2

        std::clog << parser2.convertISCAS();

        std::clog << "\n[END OF TEST 7]\n";
    } // Test 7


#ifdef USE_SOLVER_QUANTOR
    /**
     * Test 8:
     * Test before parsing and all the print functions. Satisfiable
     */
    template<>
    template<>
    void TestObject::test<8>()
    {
        std::clog << "\n[START OF TEST 8]\n";

        std::string f1 = "((a & ~b) | c)  ";
        std::string f2 = "((a | c) & (~b | c))  ";

        QBFEquiChecker parser;

        Miter miter;
        miter.addFormula("f1", f1);
        miter.addFormula("f2", f2);
        miter.connectOutputWires("f1", "f2");

        std::clog << "$$$$$$ inside miter $$$$$$\n";
        std::for_each(miter.getMap().begin(), miter.getMap().end(), QBFEquiChecker_Test::printMap);
        std::clog << std::endl;

        std::string miterFormula = miter.generateMiterFormula();
        std::clog << "Miter Formula: " << " -> " << miterFormula << std::endl;
        parser.parse(miterFormula);

        std::clog << "1) Parser Formula: " << " -> ";
        parser.printFormula(std::clog);
        std::clog << std::endl;

        parser.moveToInnerUniQuantifier("a");
        parser.moveToInnerUniQuantifier("b");
        parser.moveToInnerUniQuantifier("c");

        UT_ASSERT(parser.convertCNF());

        std::clog << "2) Parser Formula: " << " -> ";
        parser.printFormula(std::clog);
        std::clog << std::endl;
        std::clog << "Parser CNF: " << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;


        const char *dimacs_file = "test8.qdimacs";
        const char *index_file = "test8_varIdx.txt";
        std::ofstream outputFileStream(dimacs_file);
        std::ofstream outIndexStream(index_file);

        parser.printDIMACS(outputFileStream);
        std::clog << std::endl;
        parser.printIndexVarMap(outIndexStream);
        std::clog << std::endl;

        outputFileStream.close();
        outIndexStream.close();

        SolverAdaptor<QuantorAdaptor> q;
        parser.solve(q, dimacs_file);
        parser.solve(q);

        std::clog << "\n[END OF TEST 8]\n";
    } // Test 8


    /**
     * Test 9:
     * Test before parsing and all the print functions. UNSAT
     */
    template<>
    template<>
    void TestObject::test<9>()
    {
        std::clog << "\n[START OF TEST 9]\n";

        std::string x = "((a & ~b) | c)  ";
        std::string y = "((a | c) & ~f)  ";
        std::string f = "(e & ~c)";
        std::string e = "b"; // e == b is SAT; else UNSAT

        QBFEquiChecker parser;

        Miter miter;
        UT_ASSERT(miter.addFormula("x", x));
        UT_ASSERT(miter.addFormula("y", y));
        UT_ASSERT(miter.addFormula("e", e));
        UT_ASSERT(miter.addFormula("f", f));

        UT_ASSERT(miter.connectOutputWires("x", "y"));

        std::clog << "$$$$$$ inside miter $$$$$$\n";
        std::for_each(miter.getMap().begin(), miter.getMap().end(), QBFEquiChecker_Test::printMap);
        std::clog << std::endl;

        std::string miterFormula = miter.generateMiterFormula();
        std::clog << "Miter Formula: " << " -> " << miterFormula << std::endl;
        UT_ASSERT(parser.parse(miterFormula));

        std::clog << "1) Parser Formula: " << " -> ";
        parser.printFormula(std::clog);
        std::clog << std::endl;

        UT_ASSERT(parser.moveToInnerUniQuantifier("a"));
        UT_ASSERT(parser.moveToInnerUniQuantifier("b"));
        UT_ASSERT(parser.moveToInnerUniQuantifier("c"));

        UT_ASSERT(parser.convertCNF());

        std::clog << "2) Parser Formula: " << " -> ";
        parser.printFormula(std::clog);
        std::clog << std::endl;
        std::clog << "Parser CNF: " << " -> ";
        parser.printCNF(std::clog);
        std::clog << std::endl;


        const char *dimacs_file = "test9.qdimacs";
        const char *index_file = "test9_varIdx.txt";
        std::ofstream outputFileStream(dimacs_file);
        std::ofstream outIndexStream(index_file);


        parser.printDIMACS(outputFileStream);
        std::clog << std::endl;
        parser.printIndexVarMap(outIndexStream);
        std::clog << std::endl;

        outputFileStream.close();
        outIndexStream.close();

        SolverAdaptor<QuantorAdaptor> q;
        parser.solve(q, dimacs_file);
        parser.solve(q);

        std::clog << "\n[END OF TEST 9]\n";
    } // Test 9
#endif


#ifdef USE_SOLVER_CIRQIT
    /**
     * Test 10:
     * Test before parsing and all the print functions
     */
    template<>
    template<>
    void TestObject::test<10>()
    {
        std::clog << "\n[START OF TEST 10]\n";

        std::string f1 = "((a & ~b) | c)  ";
        std::string f2 = "((a | c) & (~b | c))  ";

        QBFEquiChecker parser;

        Miter miter;
        miter.addFormula("f1", f1);
        miter.addFormula("f2", f2);
        miter.connectOutputWires("f1", "f2");

        std::clog << "$$$$$$ inside miter $$$$$$\n";
        std::for_each(miter.getMap().begin(), miter.getMap().end(), QBFEquiChecker_Test::printMap);
        std::clog << std::endl;

        std::string miterFormula = miter.generateMiterFormula();
        std::clog << "Miter Formula: " << " -> " << miterFormula << std::endl;
        parser.parse(miterFormula);

        std::clog << "1) Parser Formula: " << " -> ";
        parser.printFormula(std::clog);
        std::clog << std::endl;

        parser.moveToInnerUniQuantifier("a");
        parser.moveToInnerUniQuantifier("b");
        parser.moveToInnerUniQuantifier("c");

        std::clog << "%%%%%%%%% ISCAS %%%%%%%%%\n" << std::endl;

        const char *iscas_file = "test10.iscas";
        const char *index_file = "test10_varIdx.txt";

        std::ofstream outputFileStream(iscas_file);
        std::ofstream outIdxStream(index_file);

        outputFileStream << parser.convertISCAS();
        parser.printIndexVarMap(outIdxStream);

        outputFileStream.close();
        outIdxStream.close();

        SolverAdaptor<CirQitAdaptor> q;
        parser.solve(q, CirQitAdaptor::ISCAS_FILE);

        std::clog << "\n[END OF TEST 10]\n";
    } // Test 10
#endif


    /**
     * Test 11:
     * Test before parsing and all the print functions
     */
    template<>
    template<>
    void TestObject::test<11>()
    {
        std::clog << "\n[START OF TEST 11]\n";

        std::string x = "((a & ~b) | c)  "; // (a | c) & (~b | c)
        std::string y = "((a | c) & ~f)  "; // (a | c) & (~(~b & ~c)) = (a | c) & (b | c)
        std::string f = "(e & ~c)";
        std::string e = "b & ~g"; // e == b is SAT; else UNSAT

        QBFEquiChecker parser;

        Miter miter;
        UT_ASSERT(miter.addFormula("x", x));
        UT_ASSERT(miter.addFormula("y", y));
        UT_ASSERT(miter.addFormula("e", e));
        UT_ASSERT(miter.addFormula("f", f));

        UT_ASSERT(miter.connectOutputWires("x", "y"));

        std::clog << "$$$$$$ inside miter $$$$$$\n";
        std::for_each(miter.getMap().begin(), miter.getMap().end(), QBFEquiChecker_Test::printMap);
        std::clog << std::endl;

        std::string miterFormula = miter.generateMiterFormula();
        std::clog << "Miter Formula: " << " -> " << miterFormula << std::endl;
        UT_ASSERT(parser.parse(miterFormula));

        std::clog << "1) Parser Formula: " << " -> ";
        parser.printFormula(std::clog);
        std::clog << std::endl;

        UT_ASSERT(parser.moveToOuterExtQuantifier("a"));
        UT_ASSERT(parser.moveToOuterUniQuantifier("b"));
        UT_ASSERT(parser.moveToOuterUniQuantifier("c"));
        UT_ASSERT(parser.moveToOuterExtQuantifier("g"));

        UT_ASSERT(parser.convertCNF());

        const char *out_file = "test11.qdimacs";
        const char *index_file = "test11_varIdx.txt";
        const char *miter_file = "test11_miter.txt";

        std::ofstream outputFileStream(out_file);
        std::ofstream outIdxStream(index_file);
        std::ofstream outMiterStream(miter_file);

        parser.printDIMACS(outputFileStream);
        parser.printIndexVarMap(outIdxStream);
        outMiterStream << miterFormula;

        outMiterStream.close();
        outIdxStream.close();
        outputFileStream.close();


        SolverResult_Type result = parser.solve();

        std::clog << "###### " << solverResult2Str(result)
                               << " Time taken: " << parser.timeTakenForSolving()
                               << " seconds" << std::endl;

        parser.printVarAssignments(std::clog);

        std::clog << "\n[END OF TEST 11]\n";
    } // Test 11


#ifdef USE_SOLVER_CIRQIT
    /**
     * Test 12:
     * Test for some real files. Results from CirQit 3.1.7a. SAT
     *
     * parse time: 0.071178
     * num vars = 6023
     * reduced to 3240
     * preprocess time: 0.174793
     * solve time: 3323.16
     * solution found
     * Number of decision nodes: 225133
     * Backtracks: 160194
     * Maximum search depth: 89
     */
    template<>
    template<>
    void TestObject::test<12>()
    {
        std::clog << "\n[START OF TEST 12]\n";

        QBFEquiChecker checker;
        Miter miter;

        const char *unknownCircuitFile = "ALU_8b_Verilog_dec_1.v";
        const char *knownCircuitFile = "ref_full_adder_8b_nov_29th.v";

        VerilogParser vParser, vParser2;
        UT_ASSERT(vParser.parseFile(unknownCircuitFile));
        UT_ASSERT(vParser2.parseFile(knownCircuitFile));

        std::list<std::string> inputWires = vParser.getInputWires();
        std::list<std::string> outputWires = vParser.getOutputWires();
        std::map<std::string, std::string> out_map = vParser.getWiresFormulas();
        std::map<std::string, std::string> ref_map = vParser2.getWiresFormulas();

        QBFEquiChecker_Test::AddFormulaToMiter formulaAdder(miter);
        std::for_each(out_map.begin(), out_map.end(), formulaAdder);
        std::for_each(ref_map.begin(), ref_map.end(), formulaAdder);

        UT_ASSERT(miter.connectInputWires("cin", "srcCy"));
        UT_ASSERT(miter.connectInputWires("in_a_0", "src1_0"));
        UT_ASSERT(miter.connectInputWires("in_a_1", "src1_1"));
        UT_ASSERT(miter.connectInputWires("in_a_2", "src1_2"));
        UT_ASSERT(miter.connectInputWires("in_a_3", "src1_3"));
        UT_ASSERT(miter.connectInputWires("in_a_4", "src1_4"));
        UT_ASSERT(miter.connectInputWires("in_a_5", "src1_5"));
        UT_ASSERT(miter.connectInputWires("in_a_6", "src1_6"));
        UT_ASSERT(miter.connectInputWires("in_a_7", "src1_7"));
        UT_ASSERT(miter.connectInputWires("in_b_0", "src2_0"));
        UT_ASSERT(miter.connectInputWires("in_b_1", "src2_1"));
        UT_ASSERT(miter.connectInputWires("in_b_2", "src2_2"));
        UT_ASSERT(miter.connectInputWires("in_b_3", "src2_3"));
        UT_ASSERT(miter.connectInputWires("in_b_4", "src2_4"));
        UT_ASSERT(miter.connectInputWires("in_b_5", "src2_5"));
        UT_ASSERT(miter.connectInputWires("in_b_6", "src2_6"));
        UT_ASSERT(miter.connectInputWires("in_b_7", "src2_7"));

        UT_ASSERT(miter.connectOutputWires("sum_0", "des_acc_0"));
        UT_ASSERT(miter.connectOutputWires("sum_1", "des_acc_1"));
        UT_ASSERT(miter.connectOutputWires("sum_2", "des_acc_2"));
        UT_ASSERT(miter.connectOutputWires("sum_3", "des_acc_3"));
        UT_ASSERT(miter.connectOutputWires("sum_4", "des_acc_4"));
        UT_ASSERT(miter.connectOutputWires("sum_5", "des_acc_5"));
        UT_ASSERT(miter.connectOutputWires("sum_6", "des_acc_6"));
        UT_ASSERT(miter.connectOutputWires("sum_7", "des_acc_7"));
        UT_ASSERT(miter.connectOutputWires("cout", "desCy"));

        std::clog << "\n$$$$$$$$$ generateMiterFormula() $$$$$$$$$" << std::endl;
        std::string miterFormula = miter.generateMiterFormula();
        //std::clog << miterFormula << std::endl;

        UT_ASSERT(checker.parse(miterFormula));

        const int LEN_OF_ARR = 17;
        const char *inputsArr[LEN_OF_ARR] = {"src1_0", "src1_1", "src1_2", "src1_3",
                                             "src1_4", "src1_5", "src1_6", "src1_7",
                                             "src2_0", "src2_1", "src2_2", "src2_3",
                                             "src2_4", "src2_5", "src2_6", "src2_7",
                                             "srcCy"};

        for(int i = 0; i < LEN_OF_ARR; ++i)
        {
            UT_ASSERT(checker.moveToOuterUniQuantifier(inputsArr[i]));
            inputWires.remove(inputsArr[i]);
        }

        for(std::list<std::string>::iterator iter = inputWires.begin();
            iter != inputWires.end();
            ++iter)
        {
            UT_ASSERT(checker.moveToOuterExtQuantifier(*iter));
        }

        std::ofstream outputFileStream("test12.iscas");

        outputFileStream << checker.convertISCAS();

        SolverAdaptor<CirQitAdaptor> q;
        //checker.solve(q, CirQitAdaptor::ISCAS_FILE);

        outputFileStream.close();

        std::clog << "\n[END OF TEST 12]\n";
    } // test 12


    /**
     * Test 13:
     * Test for some real files. a swap of input wires. (ISCAS) UNSAT
     *
     * parse time: 0.070889
     * num vars = 6023
     * reduced to 3242
     * preprocess time: 0.176045
     * solve time: 103.116
     * no solution found
     * Number of decision nodes: 608543
     * Backtracks: 19929
     * Maximum search depth: 88
     */
    template<>
    template<>
    void TestObject::test<13>()
    {
        std::clog << "\n[START OF TEST 13]\n";

        QBFEquiChecker checker;
        Miter miter;

        const char *unknownCircuitFile = "ALU_8b_Verilog_dec_1.v";
        const char *knownCircuitFile = "ref_full_adder_8b_nov_29th.v";

        VerilogParser vParser, vParser2;
        UT_ASSERT(vParser.parseFile(unknownCircuitFile));
        UT_ASSERT(vParser2.parseFile(knownCircuitFile));

        std::list<std::string> inputWires = vParser.getInputWires();
        std::list<std::string> outputWires = vParser.getOutputWires();
        std::map<std::string, std::string> out_map = vParser.getWiresFormulas();
        std::map<std::string, std::string> ref_map = vParser2.getWiresFormulas();

        QBFEquiChecker_Test::AddFormulaToMiter formulaAdder(miter);
        std::for_each(out_map.begin(), out_map.end(), formulaAdder);
        std::for_each(ref_map.begin(), ref_map.end(), formulaAdder);

        UT_ASSERT(miter.connectInputWires("cin", "srcCy"));
        UT_ASSERT(miter.connectInputWires("in_a_0", "src1_0"));
        UT_ASSERT(miter.connectInputWires("in_a_1", "src1_1"));
        UT_ASSERT(miter.connectInputWires("in_a_2", "src1_2"));
        UT_ASSERT(miter.connectInputWires("in_a_3", "src1_3"));
        UT_ASSERT(miter.connectInputWires("in_a_4", "src1_4"));
        UT_ASSERT(miter.connectInputWires("in_a_5", "src1_5"));
        UT_ASSERT(miter.connectInputWires("in_a_6", "src1_6"));
        UT_ASSERT(miter.connectInputWires("in_a_7", "src1_7"));
        UT_ASSERT(miter.connectInputWires("in_b_0", "src2_1"));
        UT_ASSERT(miter.connectInputWires("in_b_1", "src2_0"));
        UT_ASSERT(miter.connectInputWires("in_b_2", "src2_2"));
        UT_ASSERT(miter.connectInputWires("in_b_3", "src2_3"));
        UT_ASSERT(miter.connectInputWires("in_b_4", "src2_4"));
        UT_ASSERT(miter.connectInputWires("in_b_5", "src2_5"));
        UT_ASSERT(miter.connectInputWires("in_b_6", "src2_6"));
        UT_ASSERT(miter.connectInputWires("in_b_7", "src2_7"));

        UT_ASSERT(miter.connectOutputWires("sum_0", "des_acc_0"));
        UT_ASSERT(miter.connectOutputWires("sum_1", "des_acc_1"));
        UT_ASSERT(miter.connectOutputWires("sum_2", "des_acc_2"));
        UT_ASSERT(miter.connectOutputWires("sum_3", "des_acc_3"));
        UT_ASSERT(miter.connectOutputWires("sum_4", "des_acc_4"));
        UT_ASSERT(miter.connectOutputWires("sum_5", "des_acc_5"));
        UT_ASSERT(miter.connectOutputWires("sum_6", "des_acc_6"));
        UT_ASSERT(miter.connectOutputWires("sum_7", "des_acc_7"));
        UT_ASSERT(miter.connectOutputWires("cout", "desCy"));


        std::clog << "\n$$$$$$$$$ generateMiterFormula() $$$$$$$$$" << std::endl;
        std::string miterFormula = miter.generateMiterFormula();
        //std::clog << miterFormula << std::endl;

        UT_ASSERT(checker.parse(miterFormula));

        const int LEN_OF_ARR = 17;
        const char *inputsArr[LEN_OF_ARR] = {"src1_0", "src1_1", "src1_2", "src1_3",
                                             "src1_4", "src1_5", "src1_6", "src1_7",
                                             "src2_0", "src2_1", "src2_2", "src2_3",
                                             "src2_4", "src2_5", "src2_6", "src2_7",
                                             "srcCy"};

        for(int i = 0; i < LEN_OF_ARR; ++i)
        {
            UT_ASSERT(checker.moveToOuterUniQuantifier(inputsArr[i]));
            inputWires.remove(inputsArr[i]);
        }

        for(std::list<std::string>::iterator iter = inputWires.begin();
            iter != inputWires.end();
            ++iter)
        {
            UT_ASSERT(checker.moveToOuterExtQuantifier(*iter));
        }


        std::ofstream outputFileStream("test13.iscas");

        outputFileStream << checker.convertISCAS();

        SolverAdaptor<CirQitAdaptor> q;
        //checker.solve(q, CirQitAdaptor::ISCAS_FILE);

        outputFileStream.close();

        std::clog << "\n[END OF TEST 13]\n";
    } // test 13


    /**
     * Test 14:
     * Test for some real files. (ISCAS) a swap of output wires. UNSAT
     *
     * parse time: 0.074503
     * num vars = 6023
     * reduced to 3240
     * preprocess time: 0.195778
     * solve time: 19.6423
     * no solution found
     * Number of decision nodes: 200041
     * Backtracks: 5750
     * Maximum search depth: 89
     */
    template<>
    template<>
    void TestObject::test<14>()
    {
        std::clog << "\n[START OF TEST 14]\n";

        QBFEquiChecker checker;
        Miter miter;

        const char *unknownCircuitFile = "ALU_8b_Verilog_dec_1.v";
        const char *knownCircuitFile = "ref_full_adder_8b_nov_29th.v";

        VerilogParser vParser, vParser2;
        UT_ASSERT(vParser.parseFile(unknownCircuitFile));
        UT_ASSERT(vParser2.parseFile(knownCircuitFile));

        std::list<std::string> inputWires = vParser.getInputWires();
        std::list<std::string> outputWires = vParser.getOutputWires();
        std::map<std::string, std::string> out_map = vParser.getWiresFormulas();
        std::map<std::string, std::string> ref_map = vParser2.getWiresFormulas();

        QBFEquiChecker_Test::AddFormulaToMiter formulaAdder(miter);
        std::for_each(out_map.begin(), out_map.end(), formulaAdder);
        std::for_each(ref_map.begin(), ref_map.end(), formulaAdder);

        UT_ASSERT(miter.connectInputWires("cin", "srcCy"));
        UT_ASSERT(miter.connectInputWires("in_a_0", "src1_0"));
        UT_ASSERT(miter.connectInputWires("in_a_1", "src1_1"));
        UT_ASSERT(miter.connectInputWires("in_a_2", "src1_2"));
        UT_ASSERT(miter.connectInputWires("in_a_3", "src1_3"));
        UT_ASSERT(miter.connectInputWires("in_a_4", "src1_4"));
        UT_ASSERT(miter.connectInputWires("in_a_5", "src1_5"));
        UT_ASSERT(miter.connectInputWires("in_a_6", "src1_6"));
        UT_ASSERT(miter.connectInputWires("in_a_7", "src1_7"));
        UT_ASSERT(miter.connectInputWires("in_b_0", "src2_0"));
        UT_ASSERT(miter.connectInputWires("in_b_1", "src2_1"));
        UT_ASSERT(miter.connectInputWires("in_b_2", "src2_2"));
        UT_ASSERT(miter.connectInputWires("in_b_3", "src2_3"));
        UT_ASSERT(miter.connectInputWires("in_b_4", "src2_4"));
        UT_ASSERT(miter.connectInputWires("in_b_5", "src2_5"));
        UT_ASSERT(miter.connectInputWires("in_b_6", "src2_6"));
        UT_ASSERT(miter.connectInputWires("in_b_7", "src2_7"));

        UT_ASSERT(miter.connectOutputWires("sum_1", "des_acc_0"));
        UT_ASSERT(miter.connectOutputWires("sum_0", "des_acc_1"));
        UT_ASSERT(miter.connectOutputWires("sum_2", "des_acc_2"));
        UT_ASSERT(miter.connectOutputWires("sum_3", "des_acc_3"));
        UT_ASSERT(miter.connectOutputWires("sum_4", "des_acc_4"));
        UT_ASSERT(miter.connectOutputWires("sum_5", "des_acc_5"));
        UT_ASSERT(miter.connectOutputWires("sum_6", "des_acc_6"));
        UT_ASSERT(miter.connectOutputWires("sum_7", "des_acc_7"));
        UT_ASSERT(miter.connectOutputWires("cout", "desCy"));


        std::clog << "\n$$$$$$$$$ generateMiterFormula() $$$$$$$$$" << std::endl;
        std::string miterFormula = miter.generateMiterFormula();
        //std::clog << miterFormula << std::endl;

        UT_ASSERT(checker.parse(miterFormula));

        const int LEN_OF_ARR = 17;
        const char *inputsArr[LEN_OF_ARR] = {"src1_0", "src1_1", "src1_2", "src1_3",
                                             "src1_4", "src1_5", "src1_6", "src1_7",
                                             "src2_0", "src2_1", "src2_2", "src2_3",
                                             "src2_4", "src2_5", "src2_6", "src2_7",
                                             "srcCy"};

        for(int i = 0; i < LEN_OF_ARR; ++i)
        {
            UT_ASSERT(checker.moveToOuterUniQuantifier(inputsArr[i]));
            inputWires.remove(inputsArr[i]);
        }

        for(std::list<std::string>::iterator iter = inputWires.begin();
            iter != inputWires.end();
            ++iter)
        {
            UT_ASSERT(checker.moveToOuterExtQuantifier(*iter));
        }


        std::ofstream outputFileStream("test14.iscas");

        outputFileStream << checker.convertISCAS();

        SolverAdaptor<CirQitAdaptor> q;
        //checker.solve(q, CirQitAdaptor::ISCAS_FILE);

        outputFileStream.close();

        std::clog << "\n[END OF TEST 14]\n";
    } // test 14
#endif


    /**
     * Test 15:
     * Test for some real files (Depqbf) UNSAT as swap input wires
     */
    template<>
    template<>
    void TestObject::test<15>()
    {
        std::clog << "\n[START OF TEST 15]\n";

        QBFEquiChecker checker;
        Miter miter;

        const char *unknownCircuitFile = "ALU_8b_Verilog_dec_1.v";
        const char *knownCircuitFile = "ref_full_subtractor_8b_dec_1st.v";

        VerilogParser vParser, vParser2;
        UT_ASSERT(vParser.parseFile(unknownCircuitFile));
        UT_ASSERT(vParser2.parseFile(knownCircuitFile));

        std::list<std::string> inputWires = vParser.getInputWires();
        std::list<std::string> outputWires = vParser.getOutputWires();

        std::map<std::string, std::string> out_map = vParser.getWiresFormulas();
        std::map<std::string, std::string> ref_map = vParser2.getWiresFormulas();

        QBFEquiChecker_Test::AddFormulaToMiter formulaAdder(miter);
        std::for_each(out_map.begin(), out_map.end(), formulaAdder);
        std::for_each(ref_map.begin(), ref_map.end(), formulaAdder);


        UT_ASSERT(miter.connectInputWires("cin", "srcCy"));
        UT_ASSERT(miter.connectInputWires("in_a_0", "src1_0"));
        UT_ASSERT(miter.connectInputWires("in_a_1", "src1_1"));
        UT_ASSERT(miter.connectInputWires("in_a_2", "src1_2"));
        UT_ASSERT(miter.connectInputWires("in_a_3", "src1_3"));
        UT_ASSERT(miter.connectInputWires("in_a_4", "src1_5"));
        UT_ASSERT(miter.connectInputWires("in_a_5", "src1_4"));
        UT_ASSERT(miter.connectInputWires("in_a_6", "src1_6"));
        UT_ASSERT(miter.connectInputWires("in_a_7", "src1_7"));
        UT_ASSERT(miter.connectInputWires("in_b_0", "src2_0"));
        UT_ASSERT(miter.connectInputWires("in_b_1", "src2_1"));
        UT_ASSERT(miter.connectInputWires("in_b_2", "src2_2"));
        UT_ASSERT(miter.connectInputWires("in_b_3", "src2_3"));
        UT_ASSERT(miter.connectInputWires("in_b_4", "src2_4"));
        UT_ASSERT(miter.connectInputWires("in_b_5", "src2_5"));
        UT_ASSERT(miter.connectInputWires("in_b_6", "src2_6"));
        UT_ASSERT(miter.connectInputWires("in_b_7", "src2_7"));

        UT_ASSERT(miter.connectOutputWires("sum_0", "des_acc_0"));
        UT_ASSERT(miter.connectOutputWires("sum_1", "des_acc_1"));
        UT_ASSERT(miter.connectOutputWires("sum_2", "des_acc_2"));
        UT_ASSERT(miter.connectOutputWires("sum_3", "des_acc_3"));
        UT_ASSERT(miter.connectOutputWires("sum_4", "des_acc_4"));
        UT_ASSERT(miter.connectOutputWires("sum_5", "des_acc_5"));
        UT_ASSERT(miter.connectOutputWires("sum_6", "des_acc_6"));
        UT_ASSERT(miter.connectOutputWires("sum_7", "des_acc_7"));
        UT_ASSERT(miter.connectOutputWires("cout", "desCy"));

        std::clog << "\n$$$$$$$$$ generateMiterFormula() $$$$$$$$$" << std::endl;
        std::string miterFormula = miter.generateMiterFormula();

        UT_ASSERT(checker.parse(miterFormula));

        const int LEN_OF_ARR = 17;
        const char *inputsArr[LEN_OF_ARR] = {"src1_0", "src1_1", "src1_2", "src1_3",
                                             "src1_4", "src1_5", "src1_6", "src1_7",
                                             "src2_0", "src2_1", "src2_2", "src2_3",
                                             "src2_4", "src2_5", "src2_6", "src2_7",
                                             "srcCy"};

        for(int i = 0; i < LEN_OF_ARR; ++i)
        {
            UT_ASSERT(checker.moveToOuterUniQuantifier(inputsArr[i]));
            inputWires.remove(inputsArr[i]);
        }

        for(std::list<std::string>::iterator iter = inputWires.begin();
            iter != inputWires.end();
            ++iter)
        {
            UT_ASSERT(checker.moveToOuterExtQuantifier(*iter));
        }


        std::clog << "\n%%%%%%%%%%%% Before convertCNF %%%%%%%%%%" << std::endl;
        UT_ASSERT(checker.convertCNF());


        std::ofstream outputFileStream("test15.qdimacs");
        std::ofstream varIndexStream("varIndex15.txt");
        std::ofstream cnfformulaStream("cnfformula15.txt");

        checker.printCNF(cnfformulaStream);
        checker.printDIMACS(outputFileStream);
        checker.printIndexVarMap(varIndexStream);

        outputFileStream.close();
        varIndexStream.close();
        cnfformulaStream.close();

        SolverResult_Type result = checker.solve();
        std::clog << "###### " << solverResult2Str(result)
                  << " Time taken: " << checker.timeTakenForSolving()
                  << " seconds" << std::endl;

        std::clog << "\n[END OF TEST 15]\n";
    } // test 15



    /**
     * Test 16:
     * Test for some real files (DepQBF) SAT
     *
     * UNSAT as wrong output wires
     */
    template<>
    template<>
    void TestObject::test<16>()
    {
        std::clog << "\n[START OF TEST 16]\n";

        QBFEquiChecker checker;
        Miter miter;

        const char *unknownCircuitFile = "ALU_8b_Verilog_dec_1.v";
        const char *knownCircuitFile = "ref_full_subtractor_8b_dec_1st.v";

        VerilogParser vParser, vParser2;
        UT_ASSERT(vParser.parseFile(unknownCircuitFile));
        UT_ASSERT(vParser2.parseFile(knownCircuitFile));

        std::list<std::string> inputWires = vParser.getInputWires();
        std::list<std::string> outputWires = vParser.getOutputWires();

        std::map<std::string, std::string> out_map = vParser.getWiresFormulas();
        std::map<std::string, std::string> ref_map = vParser2.getWiresFormulas();

        QBFEquiChecker_Test::AddFormulaToMiter formulaAdder(miter);
        std::for_each(out_map.begin(), out_map.end(), formulaAdder);
        std::for_each(ref_map.begin(), ref_map.end(), formulaAdder);


        UT_ASSERT(miter.connectInputWires("cin", "srcCy"));
        UT_ASSERT(miter.connectInputWires("in_a_0", "src1_0"));
        UT_ASSERT(miter.connectInputWires("in_a_1", "src1_1"));
        UT_ASSERT(miter.connectInputWires("in_a_2", "src1_2"));
        UT_ASSERT(miter.connectInputWires("in_a_3", "src1_3"));
        UT_ASSERT(miter.connectInputWires("in_a_4", "src1_4"));
        UT_ASSERT(miter.connectInputWires("in_a_5", "src1_5"));
        UT_ASSERT(miter.connectInputWires("in_a_6", "src1_6"));
        UT_ASSERT(miter.connectInputWires("in_a_7", "src1_7"));
        UT_ASSERT(miter.connectInputWires("in_b_0", "src2_0"));
        UT_ASSERT(miter.connectInputWires("in_b_1", "src2_1"));
        UT_ASSERT(miter.connectInputWires("in_b_2", "src2_2"));
        UT_ASSERT(miter.connectInputWires("in_b_3", "src2_3"));
        UT_ASSERT(miter.connectInputWires("in_b_4", "src2_4"));
        UT_ASSERT(miter.connectInputWires("in_b_5", "src2_5"));
        UT_ASSERT(miter.connectInputWires("in_b_6", "src2_6"));
        UT_ASSERT(miter.connectInputWires("in_b_7", "src2_7"));

        UT_ASSERT(miter.connectOutputWires("sum_0", "des_acc_0"));
        UT_ASSERT(miter.connectOutputWires("sum_1", "des_acc_1"));
        UT_ASSERT(miter.connectOutputWires("sum_2", "des_acc_2"));
        UT_ASSERT(miter.connectOutputWires("sum_3", "des_acc_4"));
        UT_ASSERT(miter.connectOutputWires("sum_4", "des_acc_3"));
        UT_ASSERT(miter.connectOutputWires("sum_5", "des_acc_5"));
        UT_ASSERT(miter.connectOutputWires("sum_6", "des_acc_6"));
        UT_ASSERT(miter.connectOutputWires("sum_7", "des_acc_7"));
        UT_ASSERT(miter.connectOutputWires("cout", "desCy"));

        std::clog << "\n$$$$$$$$$ generateMiterFormula() $$$$$$$$$" << std::endl;
        std::string miterFormula = miter.generateMiterFormula();

        UT_ASSERT(checker.parse(miterFormula));

        const int LEN_OF_ARR = 17;
        const char *inputsArr[LEN_OF_ARR] = {"src1_0", "src1_1", "src1_2", "src1_3",
                                             "src1_4", "src1_5", "src1_6", "src1_7",
                                             "src2_0", "src2_1", "src2_2", "src2_3",
                                             "src2_4", "src2_5", "src2_6", "src2_7",
                                             "srcCy"};

        for(int i = 0; i < LEN_OF_ARR; ++i)
        {
            UT_ASSERT(checker.moveToOuterUniQuantifier(inputsArr[i]));
            inputWires.remove(inputsArr[i]);
        }

        for(std::list<std::string>::iterator iter = inputWires.begin();
            iter != inputWires.end();
            ++iter)
        {
            UT_ASSERT(checker.moveToOuterExtQuantifier(*iter));
        }

        UT_ASSERT(checker.convertCNF());

        std::ofstream outputFileStream("test16.qdimacs");
        std::ofstream varIndexStream("varIndex16.txt");
        std::ofstream cnfformulaStream("cnfformula16.txt");

        checker.printCNF(cnfformulaStream);
        checker.printDIMACS(outputFileStream);
        checker.printIndexVarMap(varIndexStream);

        outputFileStream.close();
        varIndexStream.close();
        cnfformulaStream.close();

        SolverResult_Type result = checker.solve();
        std::clog << "###### " << solverResult2Str(result)
                  << " Time taken: " << checker.timeTakenForSolving()
                  << " seconds" << std::endl;

        std::clog << "\n[END OF TEST 16]\n";
    } // test 16


    /**
     * Test 17:
     * Test for some real files (DepQBF) SAT
     */
    template<>
    template<>
    void TestObject::test<17>()
    {
        std::clog << "\n[START OF TEST 17]\n";

        QBFEquiChecker checker;
        Miter miter;

        const char *unknownCircuitFile = "ALU_8b_Verilog_dec_1.v";
        const char *knownCircuitFile = "ref_full_subtractor_8b_dec_1st.v";

        VerilogParser vParser, vParser2;
        UT_ASSERT(vParser.parseFile(unknownCircuitFile));
        UT_ASSERT(vParser2.parseFile(knownCircuitFile));

        std::list<std::string> inputWires = vParser.getInputWires();
        std::list<std::string> outputWires = vParser.getOutputWires();

        std::map<std::string, std::string> out_map = vParser.getWiresFormulas();
        std::map<std::string, std::string> ref_map = vParser2.getWiresFormulas();

        QBFEquiChecker_Test::AddFormulaToMiter formulaAdder(miter);
        std::for_each(out_map.begin(), out_map.end(), formulaAdder);
        std::for_each(ref_map.begin(), ref_map.end(), formulaAdder);


        UT_ASSERT(miter.connectInputWires("cin", "srcCy"));
        UT_ASSERT(miter.connectInputWires("in_a_0", "src1_0"));
        UT_ASSERT(miter.connectInputWires("in_a_1", "src1_1"));
        UT_ASSERT(miter.connectInputWires("in_a_2", "src1_2"));
        UT_ASSERT(miter.connectInputWires("in_a_3", "src1_3"));
        UT_ASSERT(miter.connectInputWires("in_a_4", "src1_4"));
        UT_ASSERT(miter.connectInputWires("in_a_5", "src1_5"));
        UT_ASSERT(miter.connectInputWires("in_a_6", "src1_6"));
        UT_ASSERT(miter.connectInputWires("in_a_7", "src1_7"));
        UT_ASSERT(miter.connectInputWires("in_b_0", "src2_0"));
        UT_ASSERT(miter.connectInputWires("in_b_1", "src2_1"));
        UT_ASSERT(miter.connectInputWires("in_b_2", "src2_2"));
        UT_ASSERT(miter.connectInputWires("in_b_3", "src2_3"));
        UT_ASSERT(miter.connectInputWires("in_b_4", "src2_4"));
        UT_ASSERT(miter.connectInputWires("in_b_5", "src2_5"));
        UT_ASSERT(miter.connectInputWires("in_b_6", "src2_6"));
        UT_ASSERT(miter.connectInputWires("in_b_7", "src2_7"));

        UT_ASSERT(miter.connectOutputWires("sum_0", "des_acc_0"));
        UT_ASSERT(miter.connectOutputWires("sum_1", "des_acc_1"));
        UT_ASSERT(miter.connectOutputWires("sum_2", "des_acc_2"));
        UT_ASSERT(miter.connectOutputWires("sum_3", "des_acc_3"));
        UT_ASSERT(miter.connectOutputWires("sum_4", "des_acc_4"));
        UT_ASSERT(miter.connectOutputWires("sum_5", "des_acc_5"));
        UT_ASSERT(miter.connectOutputWires("sum_6", "des_acc_6"));
        UT_ASSERT(miter.connectOutputWires("sum_7", "des_acc_7"));
        UT_ASSERT(miter.connectOutputWires("cout", "desCy"));

        std::clog << "\n$$$$$$$$$ generateMiterFormula() $$$$$$$$$" << std::endl;
        std::string miterFormula = miter.generateMiterFormula();

        UT_ASSERT(checker.parse(miterFormula));

        const int LEN_OF_ARR = 17;
        const char *inputsArr[LEN_OF_ARR] = {"src1_0", "src1_1", "src1_2", "src1_3",
                                             "src1_4", "src1_5", "src1_6", "src1_7",
                                             "src2_0", "src2_1", "src2_2", "src2_3",
                                             "src2_4", "src2_5", "src2_6", "src2_7",
                                             "srcCy"};

        for(int i = 0; i < LEN_OF_ARR; ++i)
        {
            UT_ASSERT(checker.moveToOuterUniQuantifier(inputsArr[i]));
            inputWires.remove(inputsArr[i]);
        }

        for(std::list<std::string>::iterator iter = inputWires.begin();
            iter != inputWires.end();
            ++iter)
        {
            UT_ASSERT(checker.moveToOuterExtQuantifier(*iter));
        }

        UT_ASSERT(checker.convertCNF());

        std::ofstream outputFileStream("test17.qdimacs");
        std::ofstream varIndexStream("varIndex17.txt");
        std::ofstream cnfformulaStream("cnfformula17.txt");

        checker.printCNF(cnfformulaStream);
        checker.printDIMACS(outputFileStream);
        checker.printIndexVarMap(varIndexStream);

        outputFileStream.close();
        varIndexStream.close();
        cnfformulaStream.close();

        SolverResult_Type result = checker.solve();
        std::clog << "###### " << solverResult2Str(result)
                  << " Time taken: " << checker.timeTakenForSolving()
                  << " seconds" << std::endl;

        checker.printVarAssignments(std::clog);

        std::clog << "\n[END OF TEST 17]\n";
    } // test 17


    /**
     * Test 18:
     * Test for same file
     */
    template<>
    template<>
    void TestObject::test<18>()
    {
        std::clog << "\n[START OF TEST 18]\n";

        QBFEquiChecker checker;
        Miter miter;

        const char *unknownCircuitFile = "ref_full_adder_8b_nov_29th_2.v";
        const char *knownCircuitFile = "full_adder_8b_ref_xor.v";

        VerilogParser vParser, vParser2;
        UT_ASSERT(vParser.parseFile(unknownCircuitFile));
        UT_ASSERT(vParser2.parseFile(knownCircuitFile));

        std::list<std::string> inputWires = vParser.getInputWires();
        std::list<std::string> outputWires = vParser.getOutputWires();

        std::map<std::string, std::string> out_map = vParser.getWiresFormulas();
        std::map<std::string, std::string> ref_map = vParser2.getWiresFormulas();

        QBFEquiChecker_Test::AddFormulaToMiter formulaAdder(miter);
        std::for_each(out_map.begin(), out_map.end(), formulaAdder);
        std::for_each(ref_map.begin(), ref_map.end(), formulaAdder);


        UT_ASSERT(miter.connectOutputWires("sum_0", "o0"));
        UT_ASSERT(miter.connectOutputWires("sum_1", "o1"));
        UT_ASSERT(miter.connectOutputWires("sum_2", "o2"));
        UT_ASSERT(miter.connectOutputWires("sum_3", "o3"));
        UT_ASSERT(miter.connectOutputWires("sum_4", "o4"));
        UT_ASSERT(miter.connectOutputWires("sum_5", "o5"));
        UT_ASSERT(miter.connectOutputWires("sum_6", "o6"));
        UT_ASSERT(miter.connectOutputWires("sum_7", "o7"));
        UT_ASSERT(miter.connectOutputWires("cout", "co_out"));

//        miter.setInputWireValue("e0", true);
//        miter.setInputWireValue("e1", false);

        std::clog << "\n$$$$$$$$$ generateMiterFormula() $$$$$$$$$" << std::endl;
        std::string miterFormula = miter.generateMiterFormula();
//        std::clog << miterFormula << std::endl;

        UT_ASSERT(checker.parse(miterFormula));

        const int LEN_OF_ARR = 17;
        const char *inputsArr[LEN_OF_ARR] = {"in_a_0", "in_a_1", "in_a_2", "in_a_3",
                                             "in_a_4", "in_a_5", "in_a_6", "in_a_7",
                                             "in_b_0", "in_b_1", "in_b_2", "in_b_3",
                                             "in_b_4", "in_b_5", "in_b_6", "in_b_7",
                                             "cin"};

        for(int i = 0; i < LEN_OF_ARR; ++i)
        {
            UT_ASSERT(checker.moveToOuterUniQuantifier(inputsArr[i]));
            inputWires.remove(inputsArr[i]);
        }

        for(std::list<std::string>::iterator iter = inputWires.begin();
            iter != inputWires.end();
            ++iter)
        {
            UT_ASSERT(checker.moveToOuterExtQuantifier(*iter));
        }

//        UT_ASSERT(checker.moveToInnerUniQuantifier("in_a_0"));
//        UT_ASSERT(checker.moveToInnerUniQuantifier("in_a_1"));
//        UT_ASSERT(checker.moveToInnerUniQuantifier("in_a_2"));
//        UT_ASSERT(checker.moveToInnerUniQuantifier("in_a_3"));
//        UT_ASSERT(checker.moveToInnerUniQuantifier("in_a_4"));
//        UT_ASSERT(checker.moveToInnerUniQuantifier("in_a_5"));
//        UT_ASSERT(checker.moveToInnerUniQuantifier("in_a_6"));
//        UT_ASSERT(checker.moveToInnerUniQuantifier("in_a_7"));
//        UT_ASSERT(checker.moveToInnerUniQuantifier("in_b_0"));
//        UT_ASSERT(checker.moveToInnerUniQuantifier("in_b_1"));
//        UT_ASSERT(checker.moveToInnerUniQuantifier("in_b_2"));
//        UT_ASSERT(checker.moveToInnerUniQuantifier("in_b_3"));
//        UT_ASSERT(checker.moveToInnerUniQuantifier("in_b_4"));
//        UT_ASSERT(checker.moveToInnerUniQuantifier("in_b_5"));
//        UT_ASSERT(checker.moveToInnerUniQuantifier("in_b_6"));
//        UT_ASSERT(checker.moveToInnerUniQuantifier("in_b_7"));
//        UT_ASSERT(checker.moveToInnerUniQuantifier("cin"));

        UT_ASSERT(checker.convertCNF());

        std::ofstream outputFileStream("test18.qdimacs");
        std::ofstream varIndexStream("varIndex18.txt");
        std::ofstream cnfformulaStream("cnfformula18.txt");

        checker.printCNF(cnfformulaStream);
        checker.printDIMACS(outputFileStream);
        checker.printIndexVarMap(varIndexStream);

        outputFileStream.close();
        varIndexStream.close();
        cnfformulaStream.close();

        SolverResult_Type result = checker.solve();
        std::clog << "###### " << solverResult2Str(result)
                  << " Time taken: " << checker.timeTakenForSolving()
                  << " seconds" << std::endl;


        std::ofstream outputFile("test18_assignments.txt");
        for(std::list<std::string>::iterator iter = inputWires.begin();
            iter != inputWires.end();
            ++iter)
        {
            Index_t index = checker.getVarIndex(*iter);
            outputFile << *iter << "\t" << checker.getVarAssignment(index) << std::endl;
        }
        outputFile.close();


        std::clog << "\n[END OF TEST 18]\n";
    } // test 18

}

