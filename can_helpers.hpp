#pragma once

#include <algorithm>
#include <cstring>
#include <stdint.h>

template <typename T>
T can_getSignal(const uint8_t* buf, const size_t bufSize, const size_t startBit, const size_t length, const bool isIntel) {
    union {
        uint64_t tempVal;
        uint8_t tempBuf[8];
        T retVal;
    } u = {0};

    const size_t startByte = startBit / 8;
    const size_t endBit = startBit + length;
    const size_t endByte = (endBit + 7) / 8;
    const size_t bytesToCopy = std::min(endByte - startByte, size_t{8});

    if (isIntel) {
        std::memcpy(u.tempBuf, buf + startByte, bytesToCopy);
    } else {
        std::memcpy(u.tempBuf + (8 - bytesToCopy), buf + startByte, bytesToCopy);
    }

    const uint64_t mask = length < 64 ? (1ULL << length) - 1ULL : -1ULL;
    const uint64_t shift = isIntel ? (startBit % 8) : (56 - startBit + (2 * (startBit % 8)));

    if (isIntel) {
        u.tempVal = (u.tempVal >> shift) & mask;
    } else {
        u.tempVal = __builtin_bswap64(u.tempVal);
        u.tempVal = (u.tempVal >> shift) & mask;
    }

    return u.retVal;
}

template <typename T>
void can_setSignal(uint8_t* buf, const size_t bufSize, const T& val, const size_t startBit, const size_t length, const bool isIntel) {
    const uint64_t mask = length < 64 ? (1ULL << length) - 1ULL : -1ULL;
    const uint64_t shift = isIntel ? (startBit % 8) : (56 - startBit + (2 * (startBit % 8)));

    union {
        uint64_t valAsBits;
        T tempVal;
    } u1 = {0};

    u1.tempVal = val;

    union {
        uint64_t data;
        uint8_t tempBuf[8];
    } u2 = {0};

    const size_t startByte = startBit / 8;
    const size_t endBit = startBit + length;
    const size_t endByte = (endBit + 7) / 8;
    const size_t bytesToCopy = std::min(endByte - startByte, size_t{8});

    if (isIntel) {
        std::memcpy(u2.tempBuf, buf + startByte, bytesToCopy);
    } else {
        std::memcpy(u2.tempBuf + (8 - bytesToCopy), buf + startByte, bytesToCopy);
    }

    if (isIntel) {
        u2.data &= ~(mask << shift);
        u2.data |= (u1.valAsBits & mask) << shift;
    } else {
        u2.data = __builtin_bswap64(u2.data);
        u2.data &= ~(mask << shift);
        u2.data |= (u1.valAsBits & mask) << shift;
        u2.data = __builtin_bswap64(u2.data);
    }

    if (isIntel) {
        std::memcpy(buf + startByte, u2.tempBuf, bytesToCopy);
    } else {
        std::memcpy(buf + startByte, u2.tempBuf + (8 - bytesToCopy), bytesToCopy);
    }
}

template <typename T>
float can_getSignal(const uint8_t* buf, const size_t bufSize, const size_t startBit, const size_t length, const bool isIntel, const float factor, const float offset) {
    T retVal = can_getSignal<T>(buf, bufSize, startBit, length, isIntel);
    return (retVal * factor) + offset;
}

template <typename T>
void can_setSignal(uint8_t* buf, const size_t bufSize, const float& val, const size_t startBit, const size_t length, const bool isIntel, const float factor, const float offset) {
    T scaledVal = static_cast<T>((val - offset) / factor);
    can_setSignal<T>(buf, bufSize, scaledVal, startBit, length, isIntel);
}
