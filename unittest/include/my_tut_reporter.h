#ifndef MY_TUT_REPORTER_H
#define MY_TUT_REPORTER_H


/**
 * @file my_tut_reporter.h
 * @author TWY
 * @date 22 Sept 2010
 */


#include <tut/tut.hpp>
#include <queue>
#include <iosfwd>


/**
 * @class MyTutReporter
 *
 * My own customized TUT reporter
 */
class MyTutReporter : public tut::callback
{
    public:
        /// Destructor
        virtual ~MyTutReporter();


        /**
         * Constructor
         * Default output stream is std::clog
         */
        MyTutReporter();


        /**
         * Constructor
         *
         * @param output_stream - The output stream to use for printing
         *
         * @warning Do not destroy the output stream which you passed in
         *          as argument
         */
        MyTutReporter(std::ostream& output_stream);


        /**
         * Called when new test run started
         */
        virtual void run_started();


        /**
         * Called when a group starts
         *
         * @param name - name of the group
         */
        virtual void group_started(const std::string& name);


        /**
         * Called when a test finishes
         *
         * @param tr - test results
         */
        virtual void test_completed(const tut::test_result& tr);


        /**
         * Called when the group is completed
         *
         * @param name - Name of the group
         */
        virtual void group_completed(const std::string& name);


        /**
         * Called when all the tests have completed running
         */
        virtual void run_completed();


    private:
        /// number of "ok" tests
        int ok_count;

        /// number of "throw exception" tests
        int exceptions_count;

        /// number of "failed" tests
        int failures_count;

        /// number of "abnormally terminated" tests
        int terminations_count;

        /// number of "give warnings" tests
        int warnings_count;

        /// number of "exception in test constructor" tests
        int test_ctr_ex_count;

        /// number of "rethrown" tests
        int rethrown_count;


        /// vector to store failed tests
        std::queue<tut::test_result> not_passed_tests;

        /// output stream for printing this report
        std::ostream& os;


        //@{
        /// Uncopyable
        MyTutReporter(const MyTutReporter&);
        MyTutReporter& operator=(const MyTutReporter&);
        //@}
};


#endif // MY_TUT_REPORTER_H

