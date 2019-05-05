#include "my_tut_reporter.h"
#include <tut/tut.hpp>

#include <cstdlib>
#include <iostream>
#include <exception>

void pauseAtExit()
{
    char tmp;

    std::clog << "Press any key to quit" << std::endl;
    std::cin >> tmp;
}



// The TUT framework
namespace
{
    // to define the extern declared in tut_runner.hpp
    tut::test_runner_singleton runner;
}



int main()
{
    //atexit(pauseAtExit);

    MyTutReporter reporter;
    runner.get().set_callback(&reporter);
    tut::test_result result;

    runner.get().run_tests("Logic_t_Test");
    runner.get().run_tests("TrueFalseNode_Test");
    runner.get().run_tests("VariableNode_Test");
    runner.get().run_tests("OperatorNotNode_Test");
    runner.get().run_tests("OperatorOrNode_Test");
    runner.get().run_tests("OperatorXorNode_Test");
    runner.get().run_tests("OperatorAndNode_Test");
    //runner.get().run_tests("FormulaParser_Test");
    runner.get().run_tests("FormulaNodeFactory_Test");
    runner.get().run_tests("RCPtr_Test");
    //runner.get().run_tests("Miter_Test");
    //runner.get().run_tests("QBFEquiChecker_Test");

 
    return 0;
}

