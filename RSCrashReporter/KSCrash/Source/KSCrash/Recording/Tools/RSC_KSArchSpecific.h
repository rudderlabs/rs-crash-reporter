//
//  KSArchSpecific.h
//
//  Created by Karl Stenerud on 2012-02-17.
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

/* Architecture-dependent defines.
 */

#ifndef HDR_RSC_KSArchSpecific_h
#define HDR_RSC_KSArchSpecific_h

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/_structs.h>

#ifdef __arm64__
#define RSC_STRUCT_MCONTEXT_L _STRUCT_MCONTEXT64
#else
#define RSC_STRUCT_MCONTEXT_L _STRUCT_MCONTEXT
#endif

#ifdef __cplusplus
}
#endif

#endif // HDR_KSArchSpecific_h
