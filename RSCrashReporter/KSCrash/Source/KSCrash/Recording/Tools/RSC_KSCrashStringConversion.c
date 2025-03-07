//
//  RSC_KSCrashStringConversion.c
//  RSCrashReporter
//
//  Created by Karl Stenerud on 31.05.22.
//  Copyright © 2022 Bugsnag Inc. All rights reserved.
//

#include "RSC_KSCrashStringConversion.h"
#include <memory.h>
#include <math.h>

// Max uint64 is 18446744073709551615
#define MAX_UINT64_DIGITS 20

size_t rsc_uint64_to_string(uint64_t value, char* dst) {
    if(value == 0) {
        dst[0] = '0';
        dst[1] = 0;
        return 1;
    }

    char buff[MAX_UINT64_DIGITS+1];
    buff[sizeof(buff)-1] = 0;
    size_t index = sizeof(buff) - 2;
    for(;;) {
        buff[index] = (value%10) + '0';
        value /= 10;
        if (value == 0) {
            break;
        }
        index--;
    }

    size_t length = sizeof(buff) - index;
    memcpy(dst, buff+index, length);
    return length - 1;
}

size_t rsc_int64_to_string(int64_t value, char* dst) {
    if (value < 0) {
        dst[0] = '-';
        return rsc_uint64_to_string((uint64_t)-value, dst+1) + 1;
    }
    return rsc_uint64_to_string((uint64_t)value, dst);
}

static char g_hexNybbles[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                              '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

size_t rsc_uint64_to_hex(uint64_t value, char* dst, int min_digits) {
    if (min_digits < 1) {
        min_digits = 1;
    } else if (min_digits > 16) {
        min_digits = 16;
    }

    char buff[MAX_UINT64_DIGITS+1];
    buff[sizeof(buff)-1] = 0;
    size_t index = sizeof(buff) - 2;
    for(int digitCount = 1;; digitCount++) {
        buff[index] = g_hexNybbles[(value&15)];
        value >>= 4;
        if (value == 0 && digitCount >= min_digits) {
            break;
        }
        index--;
    }

    size_t length = sizeof(buff) - index;
    memcpy(dst, buff+index, length);
    return length - 1;
}

/**
 * Convert a positive double to a string, allowing up to max_sig_digits.
 * To reduce the complexity of this algorithm, values with an exponent
 * other than 0 are always printed in exponential form.
 *
 * Values are rounded half-up.
 *
 * This function makes use of compiler intrinsic functions which, though not
 * officially async-safe, are actually async-safe (no allocations, locks, etc).
 *
 * This function will write a maximum of 21 characters (including the NUL) to dst.
 *
 * Returns the length of the string written to dst (not including the NUL).
 */
static size_t positive_double_to_string(const double value, char* dst, int max_sig_digits) {
    const char* const orig_dst = dst;
    if (max_sig_digits > 16) {
        max_sig_digits = 16;
    }

    if(value == 0) {
        dst[0] = '0';
        dst[1] = 0;
        return 1;
    }

    // isnan() is basically ((x) != (x))
    if(isnan(value)) {
        strlcpy(dst, "nan", 4);
        return 3;
    }

    // isinf() is a compiler intrinsic.
    if(isinf(value)) {
        strlcpy(dst, "inf", 4);
        return 3;
    }

    // log10() is a compiler intrinsic.
    int exponent = (int)log10(value);
    // Values < 1.0 must subtract 1 from exponent to handle zero wraparound.
    if (value < 1.0) {
        exponent--;
    }

    // pow() is a compiler intrinsic.
    double normalized = value / pow(10, exponent);
    // Special case for 0.1, 0.01, 0.001, etc giving a normalized value of 10.xyz.
    // We use 9.999... because 10.0 converts to a value > 10 in ieee754 binary floats.
    if (normalized > 9.99999999999999822364316059975) {
        exponent++;
        normalized = value / pow(10, exponent);
    }

    // Put all of the digits we'll use into an integer.
    double digits_and_remainder = normalized * pow(10, max_sig_digits-1);
    uint64_t digits = (uint64_t)digits_and_remainder;
    // Also round up if necessary (note: 0.5 is exact in both binary and decimal).
    if (digits_and_remainder - (double)digits >= 0.5) {
        digits++;
        // Special case: Adding one bumps us to next magnitude.
        if (digits >= (uint64_t)pow(10, max_sig_digits)) {
            exponent++;
            digits /= 10;
        }
    }

    // Extract the fractional digits.
    for (int i = max_sig_digits; i > 1; i--) {
        dst[i] = digits % 10 + '0';
        digits /= 10;
    }
    // Extract the single-digit whole part.
    dst[0] = (char)digits + '0';
    dst[1] = '.';

    // Strip off trailing zeroes, and also the '.' if there is no fractional part.
    int e_offset = max_sig_digits;
    for (int i = max_sig_digits; i > 0; i--) {
        if (dst[i] != '0') {
            if (dst[i] == '.') {
                e_offset = i;
            } else {
                e_offset = i + 1;
            }
            break;
        }
    }
    dst += e_offset;

    // Add the exponent if it's not 0.
    if (exponent != 0) {
        *dst++ = 'e';
        if (exponent >= 0) {
            *dst++ = '+';
        }
        dst += rsc_int64_to_string(exponent, dst);
    } else {
        *dst = 0;
    }

    return (size_t)(dst - orig_dst);
}

size_t rsc_double_to_string(double value, char* dst, int max_sig_digits) {
    if (max_sig_digits < 1) {
        max_sig_digits = 1;
    }
    if (value < 0) {
        dst[0] = '-';
        return positive_double_to_string(-value, dst+1, max_sig_digits) + 1;
    }
    return positive_double_to_string(value, dst, max_sig_digits);
}
