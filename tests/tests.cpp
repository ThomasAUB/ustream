#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "ustream/islot.hpp"
#include "ustream/signal.hpp"
#include "ustream/broadcast.hpp"

TEST_CASE("basic uStream tests") {

    ustream::Signal<int> sig;

    struct Slot : ustream::ISlot<int> {
        void processSignal(int i) override {
            mRXData = i;
        }
        int mRXData = 0;
    };

    Slot slot1;

    sig.emit(42);
    CHECK(slot1.mRXData == 0);
    sig.connect(slot1);

    sig.emit(42);
    CHECK(slot1.mRXData == 42);

    Slot slot2;

    sig.connect(slot2);

    sig.emit(75);
    CHECK(slot1.mRXData == 75);
    CHECK(slot2.mRXData == 75);

    slot1.disconnect();
    sig.emit(753);
    CHECK(slot1.mRXData == 75);
    CHECK(slot2.mRXData == 753);

    {
        Slot slot3;

        sig.connect(slot3);

        sig.emit(789);
        CHECK(slot1.mRXData == 75);
        CHECK(slot2.mRXData == 789);
        CHECK(slot3.mRXData == 789);
    }

    sig.emit(951);
    CHECK(slot1.mRXData == 75);
    CHECK(slot2.mRXData == 951);

    ustream::open<44>(slot1);

    ustream::broadcast<44>(12);

    CHECK(slot1.mRXData == 12);
    CHECK(slot2.mRXData == 951);

    // the slot shouldn't open as it's already connected to a signal
    CHECK(!ustream::open<44>(slot2));

    ustream::broadcast<44>(452);

    CHECK(slot1.mRXData == 452);
    CHECK(slot2.mRXData == 951);

    slot2.disconnect();
    CHECK(ustream::open<44>(slot2));

    ustream::broadcast<44>(956);

    CHECK(slot1.mRXData == 956);
    CHECK(slot2.mRXData == 956);

    // this should have no effect
    sig.emit(123);

    CHECK(slot1.mRXData == 956);
    CHECK(slot2.mRXData == 956);

}

