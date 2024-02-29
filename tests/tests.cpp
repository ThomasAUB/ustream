#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "ustream/islot.hpp"
#include "ustream/signal.hpp"
#include "ustream/broadcast.hpp"

TEST_CASE("basic uStream tests") {

    ustream::Signal<int> sig;

    struct Slot : ustream::ISlot<int> {
        Slot(int& d) :mData(d) {}
        void slotInput(int i) override {
            mData = i;
        }
        int& mData;
    };

    int receivedData1 = 0;
    Slot slot1(receivedData1);

    sig.emit(42);
    CHECK(receivedData1 == 0);
    sig.connect(slot1);

    sig.emit(42);
    CHECK(receivedData1 == 42);

    int receivedData2;
    Slot slot2(receivedData2);

    sig.connect(slot2);

    sig.emit(75);
    CHECK(receivedData1 == 75);
    CHECK(receivedData2 == 75);

    slot1.disconnect();
    sig.emit(753);
    CHECK(receivedData1 == 75);
    CHECK(receivedData2 == 753);

    int receivedData3 = 0;
    {
        Slot slot3(receivedData3);

        sig.connect(slot3);

        sig.emit(789);
        CHECK(receivedData1 == 75);
        CHECK(receivedData2 == 789);
        CHECK(receivedData3 == 789);
    }

    sig.emit(951);
    CHECK(receivedData1 == 75);
    CHECK(receivedData2 == 951);
    CHECK(receivedData3 == 789);

    ustream::open<44>(slot1);

    ustream::broadcast<44>(12);

    CHECK(receivedData1 == 12);
    CHECK(receivedData2 == 951);
    CHECK(receivedData3 == 789);

    // the slot shouldn't open as it's already connected to a signal
    CHECK(!ustream::open<44>(slot2));

    ustream::broadcast<44>(452);

    CHECK(receivedData1 == 452);
    CHECK(receivedData2 == 951);
    CHECK(receivedData3 == 789);

    slot2.disconnect();
    CHECK(ustream::open<44>(slot2));

    ustream::broadcast<44>(956);

    CHECK(receivedData1 == 956);
    CHECK(receivedData2 == 956);
    CHECK(receivedData3 == 789);

    // this should have no effect
    sig.emit(123);

    CHECK(receivedData1 == 956);
    CHECK(receivedData2 == 956);
    CHECK(receivedData3 == 789);

}

