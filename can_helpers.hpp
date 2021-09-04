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

template <typename T, size_t N>
constexpr T can_getSignal(const uint8_t (&buf)[N], const size_t startBit, const size_t length, const bool isIntel) {
    union aliasType {
        uint64_t tempVal;
        uint8_t tempBuf[8];  // This is used because memcpy into tempVal generates less optimal code
        T retVal;
    };

    aliasType tempUnion{0};
    const uint64_t mask = length < 64 ? (1ULL << length) - 1ULL : -1ULL;

    std::memcpy(tempUnion.tempBuf, &buf[startBit / 8], sizeof(tempUnion.tempVal));
    if (isIntel) {
        tempUnion.tempVal = (tempUnion.tempVal >> startBit % 8) & mask;
    } else {
        tempUnion.tempVal = __builtin_bswap64(tempUnion.tempVal);
        tempUnion.tempVal = (tempUnion.tempVal >> (64 - (startBit % 8) - length)) & mask;
    }

    return tempUnion.retVal;
}

template <typename T, size_t N>
constexpr void can_setSignal(uint8_t (&buf)[N], const T& val, const size_t startBit, const size_t length, const bool isIntel) {
    union valType {
        T tempVal;
        uint64_t valAsBits;
    };

    union dataType {
        uint8_t dataBuf[8];
        uint64_t data;
    };

    const uint64_t mask = length < 64 ? (1ULL << length) - 1ULL : -1ULL;
    const uint8_t shift = isIntel ? (startBit % 8) : (64 - startBit % 8) - length;
    const size_t firstByte = startBit / 8;

    valType valUnion{val};
    valUnion.valAsBits &= mask;

    dataType dataUnion{0};
    std::memcpy(dataUnion.dataBuf, &buf[firstByte], 8);
    if (isIntel) {
        dataUnion.data &= ~(mask << shift);
        dataUnion.data |= valUnion.valAsBits << shift;
    } else {
        dataUnion.data = __builtin_bswap64(dataUnion.data);
        dataUnion.data &= ~(mask << shift);
        dataUnion.data |= valUnion.valAsBits << shift;
        dataUnion.data = __builtin_bswap64(dataUnion.data);
    }
    std::memcpy(&buf[firstByte], dataUnion.dataBuf, 8);
}

template <typename T>
constexpr T can_getSignal(const can_Message_t& msg, const size_t startBit, const size_t length, const bool isIntel){
    return can_getSignal<T>(msg.buf, startBit, length, isIntel);
}

template <typename T>
constexpr void can_setSignal(can_Message_t& msg, const T& val, const size_t startBit, const size_t length, const bool isIntel){
    return can_setSignal<T>(msg.buf, val, startBit, length, isIntel);
}

template <typename T>
void can_setSignal(can_Message_t& msg, const T& val, const size_t startBit, const size_t length, const bool isIntel, const float factor, const float offset) {
    T scaledVal = static_cast<T>((val - offset) / factor);
    can_setSignal<T>(msg.buf, scaledVal, startBit, length, isIntel);
}

template <typename T>
float can_getSignal(can_Message_t msg, const size_t startBit, const size_t length, const bool isIntel, const float factor, const float offset) {
    T retVal = can_getSignal<T>(msg.buf, startBit, length, isIntel);
    return (retVal * factor) + offset;
}

template <typename T>
float can_getSignal(can_Message_t msg, const can_Signal_t& signal) {
    return can_getSignal<T>(msg.buf, signal.startBit, signal.length, signal.isIntel, signal.factor, signal.offset);
}

template <typename T>
void can_setSignal(can_Message_t& msg, const T& val, const can_Signal_t& signal) {
    can_setSignal(msg.buf, val, signal.startBit, signal.length, signal.isIntel, signal.factor, signal.offset);
}