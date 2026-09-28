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


namespace {

    struct ReentrantSlot final : ustream::ISlot<int> {

        void processSignal(int i) override {
            mRXData = i;
            mCount++;
            if (mOnProcess) {
                mOnProcess(*this);
            }
        }

        void disconnected() override {
            mDisconnectCount++;
        }

        int mRXData = 0;
        int mCount = 0;
        int mDisconnectCount = 0;
        void (*mOnProcess)(ReentrantSlot&) = nullptr;
        ReentrantSlot* mOther = nullptr;
        ustream::Signal<int>* mSignal = nullptr;
    };

}

TEST_CASE("slot disconnects another slot during emit") {

    ustream::Signal<int> sig;
    ReentrantSlot a, b, c;

    // slots are processed in reverse connection order : c, b, a
    sig.connect(a);
    sig.connect(b);
    sig.connect(c);

    c.mOther = &b;
    c.mOnProcess = [](ReentrantSlot& s) { s.mOther->disconnect(); };

    sig.emit(1);

    CHECK(c.mRXData == 1);
    CHECK(b.mCount == 0);
    CHECK(!b.isConnected());
    CHECK(a.mRXData == 1);
}

TEST_CASE("slot disconnects itself during emit") {

    ustream::Signal<int> sig;
    ReentrantSlot a, b;

    sig.connect(a);
    sig.connect(b);

    b.mSignal = &sig;
    b.mOnProcess = [](ReentrantSlot& s) {
        s.disconnect();
        CHECK(s.mSignal->isConnected());
    };
    a.mSignal = &sig;
    a.mOnProcess = [](ReentrantSlot& s) {
        s.disconnect();
        // only the emit cursor remains in the list
        CHECK(!s.mSignal->isConnected());
    };

    sig.emit(2);

    CHECK(a.mRXData == 2);
    CHECK(b.mRXData == 2);
    CHECK(!sig.isConnected());
}

TEST_CASE("slot destroyed during emit") {

    ustream::Signal<int> sig;
    ReentrantSlot a, c;
    auto* b = new ReentrantSlot;

    sig.connect(a);
    sig.connect(*b);
    sig.connect(c);

    c.mOther = b;
    c.mOnProcess = [](ReentrantSlot& s) { delete s.mOther; };

    sig.emit(3);

    CHECK(c.mRXData == 3);
    CHECK(a.mRXData == 3);
}

TEST_CASE("slot connected during emit is not processed by that emit") {

    ustream::Signal<int> sig;
    ReentrantSlot a, b;

    sig.connect(a);

    a.mOther = &b;
    a.mSignal = &sig;
    a.mOnProcess = [](ReentrantSlot& s) {
        if (!s.mOther->isConnected()) {
            s.mSignal->connect(*s.mOther);
        }
    };

    sig.emit(4);

    CHECK(a.mCount == 1);
    CHECK(b.mCount == 0);
    CHECK(b.isConnected());

    sig.emit(5);

    CHECK(a.mCount == 2);
    CHECK(b.mRXData == 5);
}

TEST_CASE("nested emit") {

    ustream::Signal<int> sig;
    ReentrantSlot a, b;

    sig.connect(a);
    sig.connect(b);

    b.mSignal = &sig;
    b.mOnProcess = [](ReentrantSlot& s) {
        if (s.mCount == 1) {
            s.mSignal->emit(20);
        }
    };

    sig.emit(10);

    // b gets 10 then 20 (nested), a gets 20 (nested) then 10
    CHECK(b.mCount == 2);
    CHECK(a.mCount == 2);
    CHECK(a.mRXData == 10);
    CHECK(b.mRXData == 20);
}

TEST_CASE("signal destruction disconnects its slots") {

    ReentrantSlot a, b;

    {
        ustream::Signal<int> sig;
        sig.connect(a);
        sig.connect(b);
    }

    CHECK(!a.isConnected());
    CHECK(!b.isConnected());
    CHECK(a.mDisconnectCount == 1);
    CHECK(b.mDisconnectCount == 1);
}
