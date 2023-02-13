
#include "../can_helpers.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"


enum InputMode
{
    INPUT_MODE_INACTIVE,
    INPUT_MODE_PASSTHROUGH,
    INPUT_MODE_VEL_RAMP,
    INPUT_MODE_POS_FILTER,
    INPUT_MODE_MIX_CHANNELS,
    INPUT_MODE_TRAP_TRAJ,
};

TEST_SUITE("CAN Functions")
{
    TEST_CASE("reverse")
    {
        uint8_t buf[64];
        buf[0] = 0x12;
        buf[1] = 0x34;

        std::reverse(std::begin(buf), std::end(buf));
        CHECK(buf[0] == 0x00);
        CHECK(buf[62] == 0x34);
        CHECK(buf[63] == 0x12);
    }

    TEST_CASE("getSignal")
    {
        uint8_t buf[64];
        auto val = 0x1234;
        std::memcpy(buf, &val, sizeof(val));

        val = can_getSignal<uint16_t>(buf, 0, 16, true, 1, 0);
        CHECK(val == 0x1234);

        val = can_getSignal<uint16_t>(buf, 0, 16, true);
        CHECK(val == 0x1234);

        CHECK(can_getSignal<uint16_t>(buf, 0, 12, true) == 0x234);

        val = can_getSignal<uint16_t>(buf, 0, 16, false, 1, 0);
        CHECK(val == 0x3412);

        float myFloat = 1234.6789f;
        std::memcpy(buf, &myFloat, sizeof(myFloat));
        auto floatVal = can_getSignal<float>(buf, 0, 32, true, 1, 0);
        CHECK(floatVal == 1234.6789f);

        buf[0] = 0x96;
        buf[1] = 0x00;
        buf[2] = 0x00;
        buf[3] = 0x00;
        CHECK(can_getSignal<int32_t>(buf, 0, 32, true, 0.01f, 0.0f) == 1.50f);
    }

    TEST_CASE("setSignal")
    {
        uint8_t buf[64] = {0};

        can_setSignal<uint16_t>(buf, 0x1234, 0, 16, true, 1.0f, 0.0f);
        CHECK(can_getSignal<uint16_t>(buf, 0, 16, true, 1.0f, 0.0f) == 0x1234);

        can_setSignal<uint16_t>(buf, 0xABCD, 16, 16, true, 1.0f, 0.0f);
        CHECK(can_getSignal<uint16_t>(buf, 0, 16, true, 1.0f, 0.0f) == 0x1234);
        CHECK(can_getSignal<uint16_t>(buf, 16, 16, true, 1.0f, 0.0f) == 0xABCD);

        can_setSignal<float>(buf, 1234.5678f, 32, 32, true, 1.0f, 0.0f);
        CHECK(can_getSignal<uint16_t>(buf, 0, 16, true, 1.0f, 0.0f) == 0x1234);
        CHECK(can_getSignal<uint16_t>(buf, 16, 16, true, 1.0f, 0.0f) == 0xABCD);
        CHECK(can_getSignal<float>(buf, 32, 32, true, 1.0f, 0.0f));

        can_setSignal<uint16_t>(buf, 0x1234, 0, 16, false, 1.0f, 0.0f);
        CHECK(can_getSignal<uint16_t>(buf, 0, 16, false, 1.0f, 0.0f) == 0x1234);
        CHECK(can_getSignal<uint16_t>(buf, 16, 16, true, 1.0f, 0.0f) == 0xABCD);
        CHECK(can_getSignal<float>(buf, 32, 32, true, 1.0f, 0.0f));

        can_setSignal<float>(buf, 234981.0f, 12, 32, false, 2.0f, 1.1f);
        CHECK(can_getSignal<float>(buf, 12, 32, false, 2.0f, 1.1f) == 234981.0f);

        can_setSignal<int64_t>(buf, 0xDEADBEEFCAFEBABE, 0, 64, true);
        CHECK(can_getSignal<int64_t>(buf, 0, 64, true) == 0xDEADBEEFCAFEBABE);

    }

    TEST_CASE("getSignal enums")
    {
        uint8_t buf[64];
        buf[0] = INPUT_MODE_MIX_CHANNELS;
        buf[1] = INPUT_MODE_PASSTHROUGH;
        CHECK(static_cast<InputMode>(can_getSignal<InputMode>(buf, 0, 8, true, 1, 0)) == INPUT_MODE_MIX_CHANNELS);
        CHECK(static_cast<InputMode>(can_getSignal<InputMode>(buf, 8, 8, true, 1, 0)) == INPUT_MODE_PASSTHROUGH);
    }
}