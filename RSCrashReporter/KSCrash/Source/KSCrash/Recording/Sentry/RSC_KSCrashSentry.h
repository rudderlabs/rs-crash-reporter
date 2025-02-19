//
//  RSC_KSCrashSentry.h
//
//  Created by Karl Stenerud on 2012-02-12.
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

/** Keeps watch for crashes and informs via callback when on occurs.
 */

#ifndef HDR_RSC_KSCrashSentry_h
#define HDR_RSC_KSCrashSentry_h

#ifdef __cplusplus
extern "C" {
#endif

#include "RSC_KSArchSpecific.h"
#include "RSC_KSCrashType.h"

#include <CoreFoundation/CoreFoundation.h>
#include <mach/mach_types.h>
#include <signal.h>
#include <stdbool.h>

// Some structures must be pre-allocated, so we must set an upper limit.
// Note: Memory usage = 16 bytes per thread, pre-allocated once.
#define MAX_CAPTURED_THREADS 1000

typedef CF_ENUM(unsigned, RSC_KSCrashReservedTheadType) {
    RSC_KSCrashReservedThreadTypeMachPrimary,
    RSC_KSCrashReservedThreadTypeMachSecondary,
    RSC_KSCrashReservedThreadTypeCount
};

typedef struct RSC_KSCrash_SentryContext {
    // Caller defined values. Caller must fill these out prior to installation.

    /** Called by the crash handler when a crash is detected. */
    void (*onCrash)(void *);

    /** RSCCrashSentryAttemptyDelivery */
    void (*attemptDelivery)(void);

    /**
     * The methodology used for tracing threads.
     * If true, will capture traces for all running threads
     */
    bool threadTracingEnabled;

    // Implementation defined values. Caller does not initialize these.

    /** Threads reserved by the crash handlers, which must not be suspended. */
    thread_t reservedThreads[RSC_KSCrashReservedThreadTypeCount];

    /** If true, the crash handling system is currently handling a crash.
     * When false, all values below this field are considered invalid.
     */
    bool handlingCrash;

    /** If true, a second crash occurred while handling a crash. */
    bool crashedDuringCrashHandling;

    /** If true, the registers contain valid information about the crash. */
    bool registersAreValid;

    /** True if the crash system has detected a stack overflow. */
    bool isStackOverflow;

    /** The thread that caused the problem. */
    thread_t offendingThread;

    /** Address that caused the fault. */
    uintptr_t faultAddress;

    /** The type of crash that occurred.
     * This determines which other fields are valid. */
    RSC_KSCrashType crashType;

    /** Short description of why the crash occurred. */
    const char *crashReason;

    /** The stack trace. */
    uintptr_t *stackTrace;

    /** Length of the stack trace. */
    int stackTraceLength;

    /** All threads at the time of the crash.
     * Note: This will be a kernel-allocated array that must be manually kernel-deallocated.
     */
    thread_t *allThreads;
    unsigned allThreadsCount;

    /** The run states of all threads at the time of the crash. */
    integer_t allThreadRunStates[MAX_CAPTURED_THREADS];

    /** Threads that we intend to resume after processing a crash. */
    thread_t threadsToResume[MAX_CAPTURED_THREADS];
    unsigned threadsToResumeCount;

    struct {
        /** The mach exception type. */
        int type;

        /** The mach exception code. */
        int64_t code;

        /** The mach exception subcode. */
        int64_t subcode;
    } mach;

    struct {
        /** The exception name. */
        const char *name;

        const char *userInfo;
    } NSException;

    struct {
        /** The exception name. */
        const char *name;

    } CPPException;

    struct {
        /** User context information. */
        const void *userContext;

        /** Signal information. */
        const siginfo_t *signalInfo;
    } signal;

} RSC_KSCrash_SentryContext;

/** Install crash sentry.
 *
 * @param context Contextual information for the crash handlers.
 *
 * @param crashTypes The crash types to install handlers for.
 *
 * @param onCrash Function to call when a crash occurs.
 *
 * @return which crash handlers were installed successfully.
 */
RSC_KSCrashType
rsc_kscrashsentry_installWithContext(RSC_KSCrash_SentryContext *context,
                                     RSC_KSCrashType crashTypes,
                                     void (*onCrash)(void *));

/** Uninstall crash sentry.
 *
 * @param crashTypes The crash types to install handlers for.
 */
void rsc_kscrashsentry_uninstall(RSC_KSCrashType crashTypes);

#ifdef __cplusplus
}
#endif

#endif // HDR_KSCrashSentry_h
