#include "../can_helpers.hpp"

// Extract 8-bit value
uint8_t test_8bit() {
    uint8_t buf[8] = {0xAA};
    return can_getSignal<uint8_t>(buf, 8, 0, 8, true);
}

// Extract 16-bit value
uint16_t test_16bit() {
    uint8_t buf[8] = {0x11, 0x22};
    return can_getSignal<uint16_t>(buf, 8, 0, 16, true);
}

// Extract 32-bit value
uint32_t test_32bit() {
    uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44};
    return can_getSignal<uint32_t>(buf, 8, 0, 32, true);
}

// Test simple scaling (multiply by 2)
float test_scaling_factor() {
    uint8_t buf[8] = {0x10};  // Decimal 16
    return can_getSignal<uint8_t>(buf, 8, 0, 8, true, 2.0f, 0.0f);
}

// Test simple offset (add 10)
float test_scaling_offset() {
    uint8_t buf[8] = {0x10};  // Decimal 16
    return can_getSignal<uint8_t>(buf, 8, 0, 8, true, 1.0f, 10.0f);
}

// Extract 12-bit value starting at bit 4
uint16_t test_12bit_offset4() {
    uint8_t buf[8] = {0x12, 0x34};  // 0001 0010 0011 0100
    return can_getSignal<uint16_t>(buf, 8, 4, 12, true);
}

// Extract 10-bit value starting at bit 6
uint16_t test_10bit_offset6() {
    uint8_t buf[8] = {0xAB, 0xCD};  // 1010 1011 1100 1101
    return can_getSignal<uint16_t>(buf, 8, 6, 10, true);
}

// Extract 20-bit value starting at bit 3
uint32_t test_20bit_offset3() {
    uint8_t buf[8] = {0x12, 0x34, 0x56};  // 0001 0010 0011 0100 0101 0110
    return can_getSignal<uint32_t>(buf, 8, 3, 20, true);
}

// Test scaling with non-byte-aligned value
float test_scaling_nonaligned() {
    uint8_t buf[8] = {0xA5, 0x5A};  // 1010 0101 0101 1010
    return can_getSignal<uint16_t>(buf, 8, 4, 12, true, 0.5f, 5.0f);
}
