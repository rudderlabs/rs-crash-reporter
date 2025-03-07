//
// RSC_KSJSONCodec.c
//
//  Created by Karl Stenerud on 2012-01-07.
//
//  Copyright (c) 2012 Karl Stenerud. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall remain in place
// in this source code.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "RSC_KSJSONCodec.h"
#include "RSC_KSCrashStringConversion.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// ============================================================================
#pragma mark - Configuration -
// ============================================================================

/** Set to 1 if you're also compiling RSC_KSLogger and want to use it here */
#ifndef RSC_KSJSONCODEC_UseKSLogger
#define RSC_KSJSONCODEC_UseKSLogger 1
#endif

#if RSC_KSJSONCODEC_UseKSLogger
#include "RSC_KSLogger.h"
#else
#define RSC_KSLOG_ERROR(FMT, ...)
#endif


/** The work buffer size to use when escaping string values.
 * There's little reason to change this since nothing ever gets truncated.
 */
#ifndef RSC_KSJSONCODEC_WorkBufferSize
#define RSC_KSJSONCODEC_WorkBufferSize 512
#endif

/**
 * The maximum number of significant digits when printing floats.
 * 7 (6 + 1 whole digit in exp form) is the default used by the old sprintf code.
 */
#define MAX_SIGNIFICANT_DIGITS 7

// ============================================================================
#pragma mark - Helpers -
// ============================================================================

// Compiler hints for "if" statements
#define likely_if(x) if (__builtin_expect(x, 1))
#define unlikely_if(x) if (__builtin_expect(x, 0))

/** Used for writing hex string values. */
static char rsc_g_hexNybbles[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

const char *rsc_ksjsonstringForError(const int error) {
    switch (error) {
    case RSC_KSJSON_ERROR_INVALID_CHARACTER:
        return "Invalid character";
    case RSC_KSJSON_ERROR_CANNOT_ADD_DATA:
        return "Cannot add data";
    case RSC_KSJSON_ERROR_INCOMPLETE:
        return "Incomplete data";
    case RSC_KSJSON_ERROR_INVALID_DATA:
        return "Invalid data";
    default:
        return "(unknown error)";
    }
}


// ============================================================================
#pragma mark - Encode -
// ============================================================================

// Avoiding static functions due to linker issues.

/** Add JSON encoded data to an external handler.
 * The external handler will decide how to handle the data (store/transmit/etc).
 *
 * @param context The encoding context.
 *
 * @param data The encoded data.
 *
 * @param length The length of the data.
 *
 * @return true if the data was handled successfully.
 */
#define addJSONData(CONTEXT, DATA, LENGTH)                                     \
    (CONTEXT)->addJSONData(DATA, LENGTH, (CONTEXT)->userData)

/** Escape a string portion for use with JSON and send to data handler.
 *
 * @param context The JSON context.
 *
 * @param string The string to escape and write.
 *
 * @param length The length of the string.
 *
 * @return true if the data was handled successfully.
 */
int rsc_ksjsoncodec_i_appendEscapedString(
    RSC_KSJSONEncodeContext *const context, const char *restrict const string,
    size_t length) {
    char workBuffer[RSC_KSJSONCODEC_WorkBufferSize];
    const char *const srcEnd = string + length;

    const char *restrict src = string;
    char *restrict dst = workBuffer;

    // Simple case (no escape or special characters)
    for (; src < srcEnd && *src != '\\' && *src != '\"' &&
           (unsigned char)*src >= ' ';
         src++) {
        *dst++ = *src;
    }

    // Deal with complicated case (if any)
    int result;
    for (; src < srcEnd; src++) {
        
        // If we add an escaped control character this may exceed the buffer by up to
        // 6 characters: add this chunk now, reset the buffer and carry on
        if (dst + 6 > workBuffer + RSC_KSJSONCODEC_WorkBufferSize) {
            size_t encLength = (size_t)(dst - workBuffer);
            unlikely_if((result = addJSONData(context, dst - encLength, encLength)) !=
                        RSC_KSJSON_OK) {
                return result;
            }
            dst = workBuffer;
        }
        
        switch (*src) {
        case '\\':
        case '\"':
            *dst++ = '\\';
            *dst++ = *src;
            break;
        case '\b':
            *dst++ = '\\';
            *dst++ = 'b';
            break;
        case '\f':
            *dst++ = '\\';
            *dst++ = 'f';
            break;
        case '\n':
            *dst++ = '\\';
            *dst++ = 'n';
            break;
        case '\r':
            *dst++ = '\\';
            *dst++ = 'r';
            break;
        case '\t':
            *dst++ = '\\';
            *dst++ = 't';
            break;
        default:

            // escape control chars (U+0000 - U+001F)
            // see https://www.ietf.org/rfc/rfc4627.txt

            if ((unsigned char)*src < ' ') {
                unsigned int last = (unsigned int)*src % 16;
                unsigned int first = ((unsigned int)*src - last) / 16;

                *dst++ = '\\';
                *dst++ = 'u';
                *dst++ = '0';
                *dst++ = '0';
                *dst++ = rsc_g_hexNybbles[first];
                *dst++ = rsc_g_hexNybbles[last];
            } else {
                *dst++ = *src;
            }
            break;
        }
    }
    size_t encLength = (size_t)(dst - workBuffer);
    return addJSONData(context, dst - encLength, encLength);
}

/** Escape a string for use with JSON and send to data handler.
 *
 * @param context The JSON context.
 *
 * @param string The string to escape and write.
 *
 * @param length The length of the string.
 *
 * @return true if the data was handled successfully.
 */
int rsc_ksjsoncodec_i_addEscapedString(RSC_KSJSONEncodeContext *const context,
                                       const char *restrict const string,
                                       size_t length) {
    int result = RSC_KSJSON_OK;

    // Keep adding portions until the whole string has been processed.
    size_t offset = 0;
    while (offset < length) {
        size_t toAdd = length - offset;
        unlikely_if(toAdd > RSC_KSJSONCODEC_WorkBufferSize) {
            toAdd = RSC_KSJSONCODEC_WorkBufferSize;
        }
        result = rsc_ksjsoncodec_i_appendEscapedString(context, string + offset,
                                                       toAdd);
        unlikely_if(result != RSC_KSJSON_OK) { break; }
        offset += toAdd;
    }
    return result;
}

/** Escape and quote a string for use with JSON and send to data handler.
 *
 * @param context The JSON context.
 *
 * @param string The string to escape and write.
 *
 * @param length The length of the string.
 *
 * @return true if the data was handled successfully.
 */
int rsc_ksjsoncodec_i_addQuotedEscapedString(
    RSC_KSJSONEncodeContext *const context, const char *restrict const string,
    size_t length) {
    int result;
    unlikely_if((result = addJSONData(context, "\"", 1)) != RSC_KSJSON_OK) {
        return result;
    }
    unlikely_if((result = rsc_ksjsoncodec_i_addEscapedString(
                     context, string, length)) != RSC_KSJSON_OK) {
        return result;
    }
    return addJSONData(context, "\"", 1);
}

int rsc_ksjsonbeginElement(RSC_KSJSONEncodeContext *const context,
                           const char *const name) {
    int result = RSC_KSJSON_OK;

    // Decide if a comma is warranted.
    unlikely_if(context->containerFirstEntry) {
        context->containerFirstEntry = false;
    }
    else {
        unlikely_if((result = addJSONData(context, ",", 1)) != RSC_KSJSON_OK) {
            return result;
        }
    }

    // Pretty printing
    unlikely_if(context->prettyPrint && context->containerLevel > 0) {
        unlikely_if((result = addJSONData(context, "\n", 1)) != RSC_KSJSON_OK) {
            return result;
        }
        for (int i = 0; i < context->containerLevel; i++) {
            unlikely_if((result = addJSONData(context, "    ", 4)) !=
                        RSC_KSJSON_OK) {
                return result;
            }
        }
    }

    // Add a name field if we're in an object.
    if (context->isObject[context->containerLevel]) {
        unlikely_if(name == NULL) {
            RSC_KSLOG_ERROR("Name was null inside an object");
            return RSC_KSJSON_ERROR_INVALID_DATA;
        }
        unlikely_if((result = rsc_ksjsoncodec_i_addQuotedEscapedString(
                         context, name, strlen(name))) != RSC_KSJSON_OK) {
            return result;
        }
        unlikely_if(context->prettyPrint) {
            unlikely_if((result = addJSONData(context, ": ", 2)) !=
                        RSC_KSJSON_OK) {
                return result;
            }
        }
        else {
            unlikely_if((result = addJSONData(context, ":", 1)) !=
                        RSC_KSJSON_OK) {
                return result;
            }
        }
    }
    return result;
}

int rsc_ksjsonaddRawJSONData(RSC_KSJSONEncodeContext *const context,
                             const char *const data, const size_t length) {
    return addJSONData(context, data, length);
}

int rsc_ksjsonaddBooleanElement(RSC_KSJSONEncodeContext *const context,
                                const char *const name, const bool value) {
    int result = rsc_ksjsonbeginElement(context, name);
    unlikely_if(result != RSC_KSJSON_OK) { return result; }
    if (value) {
        return addJSONData(context, "true", 4);
    } else {
        return addJSONData(context, "false", 5);
    }
}

int rsc_ksjsonaddFloatingPointElement(RSC_KSJSONEncodeContext *const context,
                                      const char *const name, double value) {
    int result = rsc_ksjsonbeginElement(context, name);
    unlikely_if(result != RSC_KSJSON_OK) { return result; }
    char buff[30];
    rsc_double_to_string(value, buff, MAX_SIGNIFICANT_DIGITS);
    return addJSONData(context, buff, strlen(buff));
}

int rsc_ksjsonaddIntegerElement(RSC_KSJSONEncodeContext *const context,
                                const char *const name, long long value) {
    int result = rsc_ksjsonbeginElement(context, name);
    unlikely_if(result != RSC_KSJSON_OK) { return result; }
    char buff[30];
    rsc_int64_to_string(value, buff);
    return addJSONData(context, buff, strlen(buff));
}

int rsc_ksjsonaddUIntegerElement(RSC_KSJSONEncodeContext *const context,
                                 const char *const name,
                                 unsigned long long value) {
    int result = rsc_ksjsonbeginElement(context, name);
    unlikely_if(result != RSC_KSJSON_OK) { return result; }
    char buff[30];
    rsc_uint64_to_string(value, buff);
    return addJSONData(context, buff, strlen(buff));
}

int rsc_ksjsonaddJSONElement(RSC_KSJSONEncodeContext *const context,
                             const char *restrict const name,
                             const char *restrict const element,
                             size_t length) {
    unlikely_if(element == NULL) {
        return rsc_ksjsonaddNullElement(context, name);
    }
    size_t idx = 0;
    while (idx < length && (element[idx] == ' ' || element[idx] == '\r' ||
                            element[idx] == '\n' || element[idx] == '\t' ||
                            element[idx] == '\f')) {
        idx++;
    }
    unlikely_if(idx >= length) {
        RSC_KSLOG_ERROR("JSON element contained no JSON data: %s", element);
        return RSC_KSJSON_ERROR_INVALID_DATA;
    }
    switch (element[idx]) {
    case '[':
    case '{':
    case '\"':
    case 'f':
    case 't':
    case 'n':
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        break;
    default:
        RSC_KSLOG_ERROR("Invalid character '%c' in: ", element[idx]);
        return RSC_KSJSON_ERROR_INVALID_DATA;
    }

    int result = rsc_ksjsonbeginElement(context, name);
    unlikely_if(result != RSC_KSJSON_OK) { return result; }
    return addJSONData(context, element, length);
}

int rsc_ksjsonaddNullElement(RSC_KSJSONEncodeContext *const context,
                             const char *const name) {
    int result = rsc_ksjsonbeginElement(context, name);
    unlikely_if(result != RSC_KSJSON_OK) { return result; }
    return addJSONData(context, "null", 4);
}

int rsc_ksjsonaddStringElement(RSC_KSJSONEncodeContext *const context,
                               const char *const name, const char *const value,
                               size_t length) {
    unlikely_if(value == NULL) {
        return rsc_ksjsonaddNullElement(context, name);
    }
    int result = rsc_ksjsonbeginElement(context, name);
    unlikely_if(result != RSC_KSJSON_OK) { return result; }
    if (length == RSC_KSJSON_SIZE_AUTOMATIC) {
        length = strlen(value);
    }
    return rsc_ksjsoncodec_i_addQuotedEscapedString(context, value, length);
}

int rsc_ksjsonbeginStringElement(RSC_KSJSONEncodeContext *const context,
                                 const char *const name) {
    int result = rsc_ksjsonbeginElement(context, name);
    unlikely_if(result != RSC_KSJSON_OK) { return result; }
    return addJSONData(context, "\"", 1);
}

int rsc_ksjsonappendStringElement(RSC_KSJSONEncodeContext *const context,
                                  const char *const value, size_t length) {
    return rsc_ksjsoncodec_i_addEscapedString(context, value, length);
}

int rsc_ksjsonendStringElement(RSC_KSJSONEncodeContext *const context) {
    return addJSONData(context, "\"", 1);
}

int rsc_ksjsonaddDataElement(RSC_KSJSONEncodeContext *const context,
                             const char *name, const char *value,
                             size_t length) {
    int result = RSC_KSJSON_OK;
    result = rsc_ksjsonbeginDataElement(context, name);
    if (result == RSC_KSJSON_OK) {
        result = rsc_ksjsonappendDataElement(context, value, length);
    }
    if (result == RSC_KSJSON_OK) {
        result = rsc_ksjsonendDataElement(context);
    }
    return result;
}

int rsc_ksjsonbeginDataElement(RSC_KSJSONEncodeContext *const context,
                               const char *const name) {
    return rsc_ksjsonbeginStringElement(context, name);
}

int rsc_ksjsonappendDataElement(RSC_KSJSONEncodeContext *const context,
                                const char *const value, size_t length) {
    const unsigned char *currentByte = (const unsigned char *)value;
    const unsigned char *end = currentByte + length;
    char chars[2];
    int result = RSC_KSJSON_OK;
    while (currentByte < end) {
        chars[0] = rsc_g_hexNybbles[(*currentByte >> 4) & 15];
        chars[1] = rsc_g_hexNybbles[*currentByte & 15];
        result = addJSONData(context, chars, sizeof(chars));
        if (result != RSC_KSJSON_OK) {
            break;
        }
        currentByte++;
    }
    return result;
}

int rsc_ksjsonendDataElement(RSC_KSJSONEncodeContext *const context) {
    return rsc_ksjsonendStringElement(context);
}

int rsc_ksjsonbeginArray(RSC_KSJSONEncodeContext *const context,
                         const char *const name) {
    likely_if(context->containerLevel >= 0) {
        int result = rsc_ksjsonbeginElement(context, name);
        unlikely_if(result != RSC_KSJSON_OK) { return result; }
    }

    context->containerLevel++;
    context->isObject[context->containerLevel] = false;
    context->containerFirstEntry = true;

    return addJSONData(context, "[", 1);
}

int rsc_ksjsonbeginObject(RSC_KSJSONEncodeContext *const context,
                          const char *const name) {
    likely_if(context->containerLevel >= 0) {
        int result = rsc_ksjsonbeginElement(context, name);
        unlikely_if(result != RSC_KSJSON_OK) { return result; }
    }

    context->containerLevel++;
    context->isObject[context->containerLevel] = true;
    context->containerFirstEntry = true;

    return addJSONData(context, "{", 1);
}

int rsc_ksjsonendContainer(RSC_KSJSONEncodeContext *const context) {
    unlikely_if(context->containerLevel <= 0) { return RSC_KSJSON_OK; }

    bool isObject = context->isObject[context->containerLevel];
    context->containerLevel--;

    // Pretty printing
    unlikely_if(context->prettyPrint && !context->containerFirstEntry) {
        int result;
        unlikely_if((result = addJSONData(context, "\n", 1)) != RSC_KSJSON_OK) {
            return result;
        }
        for (int i = 0; i < context->containerLevel; i++) {
            unlikely_if((result = addJSONData(context, "    ", 4)) !=
                        RSC_KSJSON_OK) {
                return result;
            }
        }
    }
    context->containerFirstEntry = false;
    return addJSONData(context, isObject ? "}" : "]", 1);
}

void rsc_ksjsonbeginEncode(RSC_KSJSONEncodeContext *const context,
                           bool prettyPrint,
                           RSC_KSJSONAddDataFunc addJSONDataFunc,
                           void *const userData) {
    memset(context, 0, sizeof(*context));
    context->addJSONData = addJSONDataFunc;
    context->userData = userData;
    context->prettyPrint = prettyPrint;
    context->containerFirstEntry = true;
}

int rsc_ksjsonendEncode(RSC_KSJSONEncodeContext *const context) {
    int result = RSC_KSJSON_OK;
    while (context->containerLevel > 0) {
        unlikely_if((result = rsc_ksjsonendContainer(context)) !=
                    RSC_KSJSON_OK) {
            return result;
        }
    }
    return result;
}
