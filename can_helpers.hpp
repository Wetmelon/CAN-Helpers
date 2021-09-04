#pragma once

#include <stdint.h>
#include <cstring>

struct can_Message_t {
    uint32_t id = 0x000;  // 11-bit max is 0x7ff, 29-bit max is 0x1FFFFFFF

    /**
     * Controls the IDE bit.
     */
    bool is_extended_id = false;

    /**
     * Remote Transmission Request. Controls the RTR bit in a Classical CAN
     * message. Must be false if `fd_frame` is true. 
     */
    bool rtr = false;

    /**
     * Controls the BRS bit in a CAN FD frame. If true, the payload and part of
     * the header/footer are transmitted at `data_baud_rate` instead of
     * `nominal_baud_rate`. Must be false if `fd_frame` is false.
     */
    bool bit_rate_switching = false;

    /**
     * Controls the FDF bit (aka r0 in Classical CAN). Must be false on
     * interfaces that don't support CAN FD.
     */
    bool fd_frame = false;

    uint8_t len = 8;
    uint8_t buf[64] = {0};
};

struct can_Signal_t {
    const size_t startBit;
    const size_t length;
    const bool isIntel;
    const float factor;
    const float offset;
};

template <typename T>
constexpr T can_getSignal(const can_Message_t& msg, const size_t startBit, const size_t length, bool isIntel) {
    union {
        uint64_t tempVal;
        uint8_t tempBuf[8];  // This is used because memcpy into tempVal generates less optimal code
        T retVal;
    } obj = {0};

    const uint64_t mask = length < 64 ? (1ULL << length) - 1ULL : -1ULL;

    std::memcpy(&obj.tempBuf, &msg.buf[startBit / 8], sizeof(obj.tempVal));
    if (isIntel) {
        obj.tempVal = (obj.tempVal >> startBit % 8) & mask;
    } else {
        obj.tempVal = __builtin_bswap64(obj.tempVal);
        obj.tempVal = (obj.tempVal >> (64 - (startBit % 8) - length)) & mask;
    }

    return obj.retVal;
}

template <typename T>
constexpr void can_setSignal(can_Message_t& msg, const T& val, const size_t startBit, const size_t length, const bool isIntel) {
    union {
        uint64_t valAsBits;
        T tempVal;
    } obj_val = {0};

    union {
        uint64_t data;
        uint8_t dataBuf[8];
    } obj_data = {0};

    const uint64_t mask = length < 64 ? (1ULL << length) - 1ULL : -1ULL;
    const uint8_t shift = isIntel ? (startBit % 8) : (64 - startBit % 8) - length;
    const size_t firstByte = startBit / 8;

    obj_val.tempVal = val;
    obj_val.valAsBits &= mask;

    std::memcpy(obj_data.dataBuf, &msg.buf[firstByte], 8);
    if (isIntel) {
        obj_data.data &= ~(mask << shift);
        obj_data.data |= obj_val.valAsBits << shift;
    } else {
        obj_data.data = __builtin_bswap64(obj_data.data);
        obj_data.data &= ~(mask << shift);
        obj_data.data |= obj_val.valAsBits << shift;
        obj_data.data = __builtin_bswap64(obj_data.data);
    }
    std::memcpy(&msg.buf[firstByte], obj_data.dataBuf, 8);
}

template <typename T>
void can_setSignal(can_Message_t& msg, const T& val, const size_t startBit, const size_t length, const bool isIntel, const float factor, const float offset) {
    T scaledVal = static_cast<T>((val - offset) / factor);
    can_setSignal<T>(msg, scaledVal, startBit, length, isIntel);
}

template <typename T>
float can_getSignal(can_Message_t msg, const size_t startBit, const size_t length, const bool isIntel, const float factor, const float offset) {
    T retVal = can_getSignal<T>(msg, startBit, length, isIntel);
    return (retVal * factor) + offset;
}

template <typename T>
float can_getSignal(can_Message_t msg, const can_Signal_t& signal) {
    return can_getSignal<T>(msg, signal.startBit, signal.length, signal.isIntel, signal.factor, signal.offset);
}

template <typename T>
void can_setSignal(can_Message_t& msg, const T& val, const can_Signal_t& signal) {
    can_setSignal(msg, val, signal.startBit, signal.length, signal.isIntel, signal.factor, signal.offset);
}