
#include "test_list.h"
#include "doctest.h"
#include "ustream/islot.hpp"
#include "ustream/broadcast.hpp"


void broadcastTest() {

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

    Slot slot1, slot2;



    ustream::open<44>(slot1);

    ustream::broadcast<44>(12);

    CHECK(slot1.mRXData == 12);
    CHECK(slot2.mRXData == 0);

    ustream::Signal<int> sig;
    sig.connect(slot2);
    // the slot shouldn't open as it's already connected to a signal
    CHECK(!ustream::open<44>(slot2));

    ustream::broadcast<44>(452);

    CHECK(slot1.mRXData == 452);
    CHECK(slot2.mRXData == 0);

    slot2.disconnect();
    CHECK(ustream::open<44>(slot2));

    ustream::broadcast<44>(956);

    CHECK(slot1.mRXData == 956);
    CHECK(slot2.mRXData == 956);

}