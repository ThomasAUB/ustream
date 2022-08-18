#pragma once


#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>

#include "../include/ustream/ustream.h"
#include "../include/ustream/socket.h"
#include "streams.h"

class BasicTest : public CppUnit::TestFixture { 
    
    CPPUNIT_TEST_SUITE(BasicTest);
    CPPUNIT_TEST(basic);
    CPPUNIT_TEST(callbackTest);
    CPPUNIT_TEST_SUITE_END();

    public:
    
        void setUp(){}

        void tearDown(){}

    protected:

        static bool is_sup_to_1(int i) {
            return (i>1);
        }

        static bool is_sup_to_2(int i) {
            return (i>2);
        }

        static bool is_sup_to_5(int i) {
            return (i>5);
        }

        void basic() {

            ustream::Socket<bool(int)> isSuperior_socket;

            // nothing should happen here
            isSuperior_socket(5);

            isSuperior_socket.template attach<eStreams::eBasicTest>();

            isSuperior_socket(5);

            CPPUNIT_ASSERT(!isSuperior_socket(5));

            CPPUNIT_ASSERT(ustream::Channel<eStreams::eBasicTest>::setMutable(BasicTest::is_sup_to_1));

            CPPUNIT_ASSERT(isSuperior_socket(5));

            CPPUNIT_ASSERT(ustream::Channel<eStreams::eBasicTest>::setMutable(BasicTest::is_sup_to_5));

            CPPUNIT_ASSERT(!isSuperior_socket(5));

            CPPUNIT_ASSERT(ustream::Channel<eStreams::eBasicTest>::setMutable(BasicTest::is_sup_to_1));

            CPPUNIT_ASSERT(isSuperior_socket(5));

            CPPUNIT_ASSERT(ustream::Channel<eStreams::eBasicTest>::setImmutable(BasicTest::is_sup_to_2));

            CPPUNIT_ASSERT(isSuperior_socket(5));

            CPPUNIT_ASSERT(!isSuperior_socket(2));

            CPPUNIT_ASSERT(!ustream::Channel<eStreams::eBasicTest>::setMutable(BasicTest::is_sup_to_1));

            CPPUNIT_ASSERT(isSuperior_socket(3));

            isSuperior_socket.detach();

            CPPUNIT_ASSERT(!isSuperior_socket(3));

        }

        static void multiply_by_2(int i, void(*resultCallback)(int)) {
            resultCallback(i*2);
        }

        void callbackTest() {

            ustream::Socket<void(int, void(*)(int))> multiply_socket;

            // nothing should happen here
            multiply_socket(5, 
                [](int inResult){
                    CPPUNIT_ASSERT(inResult != 10); 
                }
            );

            multiply_socket.template attach<eStreams::eBasicTest>();

            CPPUNIT_ASSERT(ustream::Channel<eStreams::eBasicTest>::setMutable(BasicTest::multiply_by_2));

            multiply_socket(5, 
                [](int inResult){
                    CPPUNIT_ASSERT(inResult == 10); 
                }
            );

            multiply_socket(5, 
                [](int inResult){
                    CPPUNIT_ASSERT(inResult != 11); 
                }
            );

        }

        
    
};
