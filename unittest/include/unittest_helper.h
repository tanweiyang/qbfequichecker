#ifndef UNITTEST_HELPER_H
#define UNITTEST_HELPER_H


// You will have to expand the macro twice
#define __UT_ASSERT_MSG(FILE_NAME, LINE_NUM, CODE) \
    ("(" #CODE ") \n\t\tat file: " #FILE_NAME " \n\t\tline: " #LINE_NUM)


#define _UT_ASSERT_MSG(FILE_NAME, LINE_NUM, CODE) \
    __UT_ASSERT_MSG(FILE_NAME, LINE_NUM, CODE)


#define UT_ASSERT_MSG(CODE)     _UT_ASSERT_MSG(__FILE__, __LINE__, CODE)


#define _UT_ASSERT(MSG, X)      tut::ensure((MSG), (X))


/**
 * @define UT_ASSERT(X)
 *
 * X refers to the expression to assert during unit testing
 */
#define UT_ASSERT(X)    _UT_ASSERT(UT_ASSERT_MSG(X), X)


#endif //UNITTEST_HELPER_H

