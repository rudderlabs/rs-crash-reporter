//
//  KSCrashSentry_Tests.m
//
//  Created by Karl Stenerud on 2013-03-09.
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


#import <XCTest/XCTest.h>

#import "RSC_KSCrashSentry.h"
#import "RSC_KSCrashSentry_Private.h"
#import "RSCDefines.h"

static void onCrash(void *context)
{
    // Do nothing
}

static RSC_KSCrash_SentryContext context;

@interface KSCrashSentry_Tests : XCTestCase @end


@implementation KSCrashSentry_Tests

- (void) setUp {
    rsc_kscrashsentry_installWithContext(&context, RSC_KSCrashTypeAll, onCrash);
}

#if RSC_HAVE_MACH_THREADS
- (void) testSuspendResumeThreads
{
    rsc_kscrashsentry_suspendThreads();
    rsc_kscrashsentry_suspendThreads();
    rsc_kscrashsentry_resumeThreads();
    rsc_kscrashsentry_resumeThreads();
}
#endif

@end
