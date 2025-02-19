//
//  RSC_KSCrashC.h
//
//  Created by Karl Stenerud on 2012-01-28.
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

/* Primary C entry point into the crash reporting system.
 */

#ifndef HDR_RSC_KSCrashC_h
#define HDR_RSC_KSCrashC_h

#ifdef __cplusplus
extern "C" {
#endif

#include "RSC_KSCrashContext.h"

#include <stdbool.h>

/** Initialize the KSCrash system. Call this once, before any other function.
 * Note: This gets called automatically by [RSC_KSCrash sharedInstance].
 */
void rsc_kscrash_init(void);

/** Install the crash reporter. The reporter will record the next crash and then
 * terminate the program.
 *
 * @param crashReportFilePath The file to store the next crash report to.
 *
 * @param recrashReportFilePath If the system crashes during crash handling,
 *                              store a second, minimal report here.
 *
 * @param stateFilePath File to store persistent state in.
 *
 * @param crashID The unique identifier to assign to the next crash report.
 *
 * @return The crash types that are being handled.
 */
RSC_KSCrashType rsc_kscrash_install(const char *const crashReportFilePath,
                                    const char *const recrashReportFilePath,
                                    const char *stateFilePath,
                                    const char *crashID);

/** Set the crash types that will be handled.
 * Some crash types may not be enabled depending on circumstances (e.g. running
 * in a debugger).
 *
 * @param crashTypes The crash types to handle.
 *
 * @return The crash types that are now behing handled. If RSC_KSCrash has been
 *         installed, the return value represents the crash sentries that were
 *         successfully installed. Otherwise it represents which sentries it
 *         will attempt to activate when RSC_KSCrash installs.
 */
RSC_KSCrashType rsc_kscrash_setHandlingCrashTypes(RSC_KSCrashType crashTypes);

/** Reinstall the crash reporter. Useful for resetting the crash reporter
 * after a "soft" crash.
 *
 * @param crashReportFilePath The file to store the next crash report to.
 *
 * @param recrashReportFilePath If the system crashes during crash handling,
 *                              store a second, minimal report here.
 *
 * @param stateFilePath File to store persistent state in.
 *
 * @param crashID The unique identifier to assign to the next crash report.
 */
void rsc_kscrash_reinstall(const char *const crashReportFilePath,
                           const char *const recrashReportFilePath,
                           const char *const stateFilePath,
                           const char *const crashID);

/** Set the callback to invoke upon a crash.
 *
 * WARNING: Only call async-safe functions from this function! DO NOT call
 * Objective-C methods!!!
 *
 * @param onCrashNotify Function to call during a crash report to give the
 *                      callee an opportunity to add to the report.
 *                      NULL = ignore.
 *
 * Default: NULL
 */
void rsc_kscrash_setCrashNotifyCallback(
    const RSC_KSReportWriteCallback onCrashNotify);

void rsc_kscrash_setThreadTracingEnabled(bool threadTracingEnabled);

/**
 * The current crash context
 */
RSC_KSCrash_Context *crashContextRSC(void);

#ifdef __cplusplus
}
#endif

#endif // HDR_KSCrashC_h
