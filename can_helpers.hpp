#pragma once

#include <cstring>
#include <stdint.h>

template <typename T>
T can_getSignal(const uint8_t (&buf)[8], const size_t startBit, const size_t length, const bool isIntel) {
    union {
        uint64_t tempVal;
        uint8_t tempBuf[8]; // This is used because memcpy into tempVal generates less optimal code
        T retVal;
    };

    const uint64_t mask = length < 64 ? (1ULL << length) - 1ULL : -1ULL;
    const uint64_t shift = isIntel ? startBit : (56 - startBit + (startBit % 8));

    std::memcpy(tempBuf, buf, 8);
    if (isIntel) {
        tempVal = (tempVal >> shift) & mask;
    } else {
        tempVal = __builtin_bswap64(tempVal);
        tempVal = (tempVal >> shift) & mask;
    }

    return retVal;
}

template <typename T>
void can_setSignal(uint8_t (&buf)[8], const T& val, const size_t startBit, const size_t length, const bool isIntel) {

    const uint64_t mask = length < 64 ? (1ULL << length) - 1ULL : -1ULL;
    const uint64_t shift = isIntel ? startBit : (56 - startBit + (startBit % 8));

    union {
        uint64_t valAsBits;
        T tempVal;
    };

    tempVal = val;

    union {
        uint64_t data;
        uint8_t tempBuf[8];
    };

    std::memcpy(tempBuf, buf, 8);
    if (isIntel) {
        data &= ~(mask << shift);
        data |= valAsBits << shift;
    } else {
        data = __builtin_bswap64(data);
        data &= ~(mask << shift);
        data |= valAsBits << shift;
        data = __builtin_bswap64(data);
    }

    std::memcpy(buf, tempBuf, 8);
}

template <typename T>
float can_getSignal(const uint8_t (&buf)[8], const size_t startBit, const size_t length, const bool isIntel, const float factor, const float offset) {
    T retVal = can_getSignal<T>(buf, startBit, length, isIntel);
    return (retVal * factor) + offset;
}

template <typename T>
void can_setSignal(uint8_t (&buf)[8], const float& val, const size_t startBit, const size_t length, const bool isIntel, const float factor, const float offset) {
    T scaledVal = static_cast<T>((val - offset) / factor);
    can_setSignal<T>(buf, scaledVal, startBit, length, isIntel);
}
