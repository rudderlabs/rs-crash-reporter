//
//  RSC_KSCrashState.h
//
//  Created by Karl Stenerud on 2012-02-05.
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

/* Manages persistent state information useful for crash reporting such as
 * number of sessions, session length, etc.
 */

#ifndef HDR_RSC_KSCrashState_h
#define HDR_RSC_KSCrashState_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct {

    // Saved data

    /** Total time elapsed in the foreground since launch. */
    double foregroundDurationSinceLaunch;

    /** Total time backgrounded elapsed since launch. */
    double backgroundDurationSinceLaunch;

    /** If true, the application crashed on the previous launch. */
    bool crashedLastLaunch;

    // Live data

    /** If true, the application crashed on this launch. */
    bool crashedThisLaunch;

    /** Timestamp for when the app was launched (mach_absolute_time()) */
    uint64_t appLaunchTime;

    /** Timestamp for when foregroundDurationSinceLastCrash or
     * backgroundDurationSinceLastCrash was last updated (mach_absolute_time()) */
    uint64_t lastUpdateDurationsTime;

    /** If true, the application is currently in the foreground. */
    bool applicationIsInForeground;

} RSC_KSCrash_State;

/** Initialize the state monitor.
 *
 * @param stateFilePath Where to store on-disk representation of state.
 *
 * @param state Where to store in-memory representation of state.
 *
 * @return true if initialization was successful.
 */
bool rsc_kscrashstate_init(const char *stateFilePath, RSC_KSCrash_State *state);

/** Notify the crash reporter of the application foreground/background state.
 *
 * @param isInForeground true if the application is in the foreground, false if
 *                 it is in the background.
 */
void rsc_kscrashstate_notifyAppInForeground(bool isInForeground);

/** Notify the crash reporter that the application has crashed.
 */
void rsc_kscrashstate_notifyAppCrash(void);

/** Read-only access into the current state.
 */
const RSC_KSCrash_State *rsc_kscrashstate_currentState(void);

/**
 * Updates the stats for duration in foreground/background. This needs to
 * be updated whenever an error report is captured.
 */
void rsc_kscrashstate_updateDurationStats(void);

#ifdef __cplusplus
}
#endif

#endif // HDR_KSCrashState_h
