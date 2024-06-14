#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "ustream/islot.hpp"
#include "ustream/signal.hpp"
#include "ustream/broadcast.hpp"

TEST_CASE("basic uStream tests") {

    ustream::Signal<int> sig;

    struct Slot : ustream::ISlot<int> {

        void connected() override {
            connectState = true;
        }

        void disconnected() override {
            connectState = false;
        }

        void processSignal(int i) override {
            mRXData = i;
        }

        int mRXData = 0;
        bool connectState = false;
    };

    Slot slot1;

    sig.emit(42);

    CHECK(slot1.mRXData == 0);

    CHECK(!slot1.connectState);

    sig.connect(slot1);

    CHECK(slot1.connectState);

    sig.emit(42);

    CHECK(slot1.mRXData == 42);

    Slot slot2;

    CHECK(!slot2.connectState);

    sig.connect(slot2);

    CHECK(slot2.connectState);

    sig.emit(75);

    CHECK(slot1.mRXData == 75);
    CHECK(slot2.mRXData == 75);

    slot1.disconnect();

    CHECK(!slot1.connectState);

    sig.emit(753);
    CHECK(slot1.mRXData == 75);
    CHECK(slot2.mRXData == 753);

    {
        Slot slot3;

        CHECK(!slot3.connectState);

        sig.connect(slot3);

        CHECK(slot3.connectState);

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

