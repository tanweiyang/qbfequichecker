#include "my_tut_reporter.h"

#include <ctime>
#include <iostream>

namespace
{
    void newTestPrintout(std::ostream& os)
    {
        time_t t = time(0);

        os << "\n***********************************************************\n"
           << "\n"
           << "\t\t TUT FRAMEWORK UNIT TEST\n"
           << "\n"
           << "\n"
           << "\ton: " << __DATE__ << "\n"
           << "\tat: " << ctime(&t) << "\n"
           << "\n*******************************************************\n\n\n";

        return;
    }
}


std::ostream& operator<<(std::ostream& os, const tut::test_result& tr)
{
    switch(tr.result)
    {
        case tut::test_result::ok:
            os << "[test " << tr.test << " -> OK]";
            break;
        case tut::test_result::fail:
            os << "[test " << tr.test << " -> FAIL]";
            break;
        case tut::test_result::ex_ctor:
            os << "[test " << tr.test << " -> Test Constructor Exception]";
            break;
        case tut::test_result::ex:
            os << "[test " << tr.test << " -> Exception]";
            break;
        case tut::test_result::warn:
            os << "[test " << tr.test << " -> Warning]";
            break;
        case tut::test_result::term:
            os << "[test " << tr.test << " -> Terminate]";
            break;
        case tut::test_result::rethrown:
            os << "[test " << tr.test << " -> Rethrown]";
            break;
        default:
            // possible?? should i just throw errors
            os << "!!! ERROR IN REPORTER AT " << __LINE__;
            break;
    }

    return os;
}


MyTutReporter::MyTutReporter() :
    ok_count(0),
    exceptions_count(0),
    failures_count(0),
    terminations_count(0),
    warnings_count(0),
    test_ctr_ex_count(0),
    rethrown_count(0),
    os(std::clog)
{
}


MyTutReporter::MyTutReporter(std::ostream& output_stream) :
    ok_count(0),
    exceptions_count(0),
    failures_count(0),
    terminations_count(0),
    warnings_count(0),
    test_ctr_ex_count(0),
    rethrown_count(0),
    os(output_stream)
{
}


MyTutReporter::~MyTutReporter()
{
}


void MyTutReporter::run_started()
{
    newTestPrintout(os);

    os << "===================================================\n"
       << "                 START A NEW TEST                  \n"
       << "===================================================\n"
       << std::endl;
}


void MyTutReporter::group_started(const std::string& name)
{
    os << "----------------------------------\n"
       << " START: " << name << "\n"
       << "----------------------------------"
       << std::endl;
}


void MyTutReporter::test_completed(const tut::test_result& tr)
{
    os << "#####" << tr.group << ": "
       << tr << "\n"
       << std::endl;

    switch(tr.result)
    {
        case tut::test_result::ok:
            ++ok_count;
            break;
        case tut::test_result::fail:
            ++failures_count;
            break;
        case tut::test_result::ex_ctor:
            ++test_ctr_ex_count;
            break;
        case tut::test_result::ex:
            ++exceptions_count;
            break;
        case tut::test_result::warn:
            ++warnings_count;
            break;
        case tut::test_result::term:
            ++terminations_count;
            break;
        case tut::test_result::rethrown:
            ++rethrown_count;
            break;
        default:
            // possible?? should i just throw errors
            os << "!!! ERROR IN REPORTER AT " << __LINE__;
            break;
    }

    if(tr.result != tut::test_result::ok)
    {
        not_passed_tests.push(tr);
    }
}


void MyTutReporter::group_completed(const std::string& name)
{
    os << "----------------------------------\n"
       << " END: " << name << "\n"
       << "----------------------------------\n"
       << std::endl;
}


void MyTutReporter::run_completed()
{
    os << std::endl;
    os << "===================================================\n"
       << "                   TEST SUMMARY                    \n"
       << "===================================================\n"
       << std::endl;

    while(!not_passed_tests.empty())
    {
        tut::test_result tr = not_passed_tests.front();

        os << "\n"
           << "---> " << "test group: " << tr.group
           << ", test: test<" << tr.test << ">"
           << (!tr.name.empty() ?
                (std::string(" : ") + tr.name) :
                "")
           << "\n";

#ifdef TUT_USE_POSIX
        if(tr.pid != getpid())
        {
            os << "    child pid: " << tr.pid << "\n";
        }
#endif

        os << "    problems: " ;

        switch(tr.result)
        {
            case tut::test_result::fail:
                os << "assertion failed" << std::endl;
                break;

            case tut::test_result::ex:
            case tut::test_result::ex_ctor:
                os << "unexpected exception" << std::endl;
                if(tr.exception_typeid != "")
                {
                    os << "    exception typeid: "
                       << tr.exception_typeid << std::endl;
                }
                break;

            case tut::test_result::warn:
                os << "test passed, "
                   << "but cleanup code (destructor) raised an exception"
                   << std::endl;
                break;

            case tut::test_result::term:
                os << "would be terminated" << std::endl;
                break;

            case tut::test_result::rethrown:
                os << "assertion failed in child" << std::endl;
                break;

            default:
                break;
        }

        if(!tr.message.empty())
        {
            if(tr.result == tut::test_result::fail)
            {
                os << "    failed assertion: " << tr.message << "\n";
            }
            else
            {
                os << "    message: " << tr.message << "\n";
            }
        }

        not_passed_tests.pop();

    } // while not_passed_tests is not empty


    os << "\n***********************\n";

    if(terminations_count)
    {
        os << "terminations: " << terminations_count << "\n";
    }
    if(exceptions_count)
    {
        os << "exceptions: " << exceptions_count << "\n";
    }
    if(failures_count)
    {
        os << "failures: " << failures_count << "\n";
    }
    if(warnings_count)
    {
        os << "warnings: " << warnings_count << "\n";
    }

    os << "ok: " << ok_count << "\n";

    os << "\n" << std::endl;
}

