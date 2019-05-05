#include "rcptr.h"
#include "unittest_helper.h"
#include <tut/tut.hpp>
#include <iostream>
#include <string>


using namespace QBFEquiChecker_NS;


namespace
{
    struct RCPtr_Test
    {
        RCPtr_Test()
        {
            std::clog << "RCPtr_Test starting...\n";
        }

        ~RCPtr_Test()
        {
            std::clog << "RCPtr_Test closing...\n";
           
        }
    };


    template<typename T>
    void swap_ptr(RCPtr<T>& lhs, RCPtr<T>& rhs)
    {
        RCPtr<T> tmp = lhs;
        lhs = rhs;
        rhs = tmp;
    }


    struct TestClass
    {
        explicit TestClass(int data) : data_(data){}
        int data_;
    };
}


namespace
{
    tut::test_group<RCPtr_Test> 
        testGroup("RCPtr_Test");
}


namespace tut
{
    typedef tut::test_group<RCPtr_Test>::object TestObject;


	/**
     * Test 1: 
     * Test reference counting
     */
    template<>
    template<>
    void TestObject::test<1>()
    {
        std::clog << "\n[START OF TEST 1]\n";

        RCPtr<int> ptr1;
        UT_ASSERT(ptr1.getRefCount() == 1);
        RCPtr<int> ptr2(ptr1);
        UT_ASSERT(ptr1.getRefCount() == 2);
        UT_ASSERT(ptr2.getRefCount() == 2);
        UT_ASSERT(ptr1.get() == ptr2.get());

        RCPtr<int> ptr3(new int(1));
        ptr3 = ptr2;
        UT_ASSERT(ptr2.get() == ptr3.get());
        UT_ASSERT(ptr1.getRefCount() == 3);
        UT_ASSERT(ptr2.getRefCount() == 3);
        UT_ASSERT(ptr2.getRefCount() == 3);

        
        swap_ptr(ptr1, ptr2);

        std::clog << "\n[END OF TEST 1]\n";
    } // test 1


    /**
     * Test 2:
     * Test * and -> and get()
     */
    template<>
    template<>
    void TestObject::test<2>()
    {
        std::clog << "\n[START OF TEST 2]\n";

        RCPtr<TestClass> ptr1(new TestClass(2));
        RCPtr<TestClass> ptr2(new TestClass(3));
        UT_ASSERT(ptr1.getRefCount() == 1);
        UT_ASSERT(ptr2.getRefCount() == 1);
        UT_ASSERT(ptr1->data_ == 2);
        UT_ASSERT(ptr2->data_ == 3);
        UT_ASSERT((*ptr1).data_ == 2);
        UT_ASSERT((*ptr2).data_ == 3);
        UT_ASSERT(ptr1.get() != ptr2.get());

        swap_ptr(ptr1, ptr2);

        UT_ASSERT(ptr1.getRefCount() == 1);
        UT_ASSERT(ptr2.getRefCount() == 1);
        UT_ASSERT(ptr1->data_ == 3);
        UT_ASSERT(ptr2->data_ == 2);
        UT_ASSERT((*ptr1).data_ == 3);
        UT_ASSERT((*ptr2).data_ == 2);

        swap_ptr(ptr1, ptr1);

        UT_ASSERT(ptr1.getRefCount() == 1);
        UT_ASSERT(ptr2.getRefCount() == 1);
        UT_ASSERT(ptr1->data_ == 3);
        UT_ASSERT(ptr2->data_ == 2);
        UT_ASSERT((*ptr1).data_ == 3);
        UT_ASSERT((*ptr2).data_ == 2);


        std::clog << "\n[END OF TEST 2]\n";
    } // test 2


    /**
      * Test 3:
      * Test reference counting
      */
     template<>
     template<>
     void TestObject::test<3>()
     {
         std::clog << "\n[START OF TEST 3]\n";

         RCPtr<TestClass> ptr1(new TestClass(2));
         UT_ASSERT(ptr1.getRefCount() == 1);
         RCPtr<TestClass> ptr2(ptr1);
         UT_ASSERT(ptr2.get() == ptr1.get());
         UT_ASSERT(ptr1.getRefCount() == 2);
         UT_ASSERT(ptr2.getRefCount() == 2);
         RCPtr<TestClass> ptr3(new TestClass(3));
         UT_ASSERT(ptr3.getRefCount() == 1);
         ptr3 = ptr1;
         UT_ASSERT(ptr3.get() == ptr1.get());
         UT_ASSERT(ptr1.getRefCount() == 3);
         UT_ASSERT(ptr2.getRefCount() == 3);
         UT_ASSERT(ptr3.getRefCount() == 3);

         struct Tmptmp
         {
             Tmptmp(RCPtr<TestClass> x) : x_(x){}

             RCPtr<TestClass> x_;
         };

         {
             Tmptmp tmptmp(ptr3);

             UT_ASSERT(ptr1.getRefCount() == 4);
             UT_ASSERT(ptr2.getRefCount() == 4);
             UT_ASSERT(ptr3.getRefCount() == 4);

             swap_ptr(ptr2, ptr1);
             UT_ASSERT(ptr1.getRefCount() == 4);
             UT_ASSERT(ptr2.getRefCount() == 4);
             UT_ASSERT(ptr3.getRefCount() == 4);
         }

         UT_ASSERT(ptr1.getRefCount() == 3);
         UT_ASSERT(ptr2.getRefCount() == 3);
         UT_ASSERT(ptr3.getRefCount() == 3);

         UT_ASSERT(ptr1->data_ == 2);
         UT_ASSERT(ptr2->data_ == 2);
         UT_ASSERT((*ptr3).data_ == 2);


         RCPtr<TestClass> ptr4;
         ptr2 = ptr4;
         UT_ASSERT(ptr1.getRefCount() == 2);
         UT_ASSERT(ptr2.getRefCount() == 2);
         UT_ASSERT(ptr3.getRefCount() == 2);
         UT_ASSERT(ptr4.getRefCount() == 2);

         std::clog << "\n[END OF TEST 3]\n";
     } // test 3

}

