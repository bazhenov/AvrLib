#ifndef MOCKS_HPP_
#define MOCKS_HPP_

#include "HAL/Atmel/InterruptHandlers.hpp"
#include "Time/Units.hpp"
#include <gtest/gtest.h>
#include "Fifo.hpp"

namespace Mocks {

using namespace Time;
using namespace HAL::Atmel;

struct MockComparator {
    constexpr static uint8_t prescalerPower2 = 3; // prescaler is 2^3 = 8

    typedef uint8_t value_t;
    typedef Int_TIMER0_COMPA_ INT;

    value_t value = 0;
    value_t target = 0;
    bool isInterruptOn = false;

    void advance() {
        value++;
    }

    template<typename duration_t>
    void advance(duration_t duration) {
        value += value_t(toCountsOn<MockComparator>(duration));
    }

    void interruptOff() {
        isInterruptOn = false;
    }

    void interruptOn() {
        isInterruptOn = true;
    }

    value_t getValue() {
        return value;
    }

    void setTarget(value_t t) {
        target = t;
    }
};

struct MockPin {
    typedef Int_INT0_ INT;

    bool isOutput = true;
    bool high = false;
    bool isInterruptOn = false;

    void configureAsInputWithPullup() {
        isOutput = false;
    }

    void configureAsInputWithoutPullup() {
        isOutput = false;
    }

    void configureAsOutput() {
        isOutput = true;
    }

    void setLow() {
        EXPECT_TRUE(isOutput);
        high = false;
    }

    void setHigh() {
        EXPECT_TRUE(isOutput);
        high = true;
    }

    void configureAsOutputLow() {
        configureAsOutput();
        setLow();
    }

    void configureAsOutputHigh() {
        configureAsOutput();
        setHigh();
    }

    bool isHigh() {
        return high;
    }

    void interruptOnChange() {
        isInterruptOn = true;
    }

    void interruptOnRising() {
        isInterruptOn = true;
    }

    void interruptOff() {
        isInterruptOn = false;
    }
};

template <uint8_t p = 10>
struct MockRealTimerPrescaled {
    constexpr static uint8_t prescalerPower2 = p;
    typedef uint8_t value_t;
    constexpr static uint8_t maximum = 255; // 8-bit timer

    uint32_t c = 0;
    int slept = 0;

    void haveSlept(Milliseconds<> millis) {
        slept += millis.getValue();
    }

    template<typename duration_t>
    void advance(duration_t duration) {
        c += uint32_t(toCountsOn<MockRealTimerPrescaled<p>>(duration));
    }

    uint32_t counts() {
        return c;
    }
};

typedef MockRealTimerPrescaled<10> MockRealTimer;

}

struct MockTWI {
    Fifo<32> out;
    Fifo<32> in;
    uint8_t writeAddress;
    uint8_t readAddress;

    template <typename... types> void write(uint8_t address, types... args) {
        writeAddress = address;
        out.write(args...);
    }
    template <typename... types> void read(uint8_t address, types... args) {
        readAddress = address;
        in.read(args...);
    }
};

#endif /* MOCKS_HPP_ */