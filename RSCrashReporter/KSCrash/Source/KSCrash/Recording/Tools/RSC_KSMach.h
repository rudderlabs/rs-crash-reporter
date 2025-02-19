//
//  KSMach.h
//
//  Created by Karl Stenerud on 2012-01-29.
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

/* Utility functions for querying the mach kernel.
 */

#ifndef HDR_RSC_KSMach_h
#define HDR_RSC_KSMach_h

#ifdef __cplusplus
extern "C" {
#endif

#include "RSC_KSArchSpecific.h"
#include "RSCDefines.h"

#include <mach/mach.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/ucontext.h>

// ============================================================================
#pragma mark - General Information -
// ============================================================================

/** Get the current CPU architecture.
 *
 * @return The current architecture.
 */
const char *rsc_ksmachcurrentCPUArch(void);

/** Get the name of a mach exception.
 *
 * @param exceptionType The exception type.
 *
 * @return The exception's name or NULL if not found.
 */
const char *rsc_ksmachexceptionName(exception_type_t exceptionType);

/** Get the name of a mach kernel return code.
 *
 * @param returnCode The return code.
 *
 * @return The code's name or NULL if not found.
 */
const char *rsc_ksmachkernelReturnCodeName(const kern_return_t returnCode);

// ============================================================================
#pragma mark - Thread State Info -
// ============================================================================

#if RSC_HAVE_MACH_THREADS
/** Fill in state information about a thread.
 *
 * @param thread The thread to get information about.
 *
 * @param state Pointer to buffer for state information.
 *
 * @param flavor The kind of information to get (arch specific).
 *
 * @param stateCount Number of entries in the state information buffer.
 *
 * @return true if state fetching was successful.
 */
bool rsc_ksmachfillState(thread_t thread, thread_state_t state,
                         thread_state_flavor_t flavor,
                         mach_msg_type_number_t stateCount);
#endif

/** Get the frame pointer for a machine context.
 * The frame pointer marks the top of the call stack.
 *
 * @param machineContext The machine context.
 *
 * @return The context's frame pointer.
 */
uintptr_t rsc_ksmachframePointer(const RSC_STRUCT_MCONTEXT_L *machineContext);

/** Get the current stack pointer for a machine context.
 *
 * @param machineContext The machine context.
 *
 * @return The context's stack pointer.
 */
uintptr_t rsc_ksmachstackPointer(const RSC_STRUCT_MCONTEXT_L *machineContext);

/** Get the address of the instruction about to be, or being executed by a
 * machine context.
 *
 * @param machineContext The machine context.
 *
 * @return The context's next instruction address.
 */
uintptr_t
rsc_ksmachinstructionAddress(const RSC_STRUCT_MCONTEXT_L *machineContext);

/** Get the address stored in the link register (arm only). This may
 * contain the first return address of the stack.
 *
 * @param machineContext The machine context.
 *
 * @return The link register value.
 */
uintptr_t rsc_ksmachlinkRegister(const RSC_STRUCT_MCONTEXT_L *machineContext);

/** Get the address whose access caused the last fault.
 *
 * @param machineContext The machine context.
 *
 * @return The faulting address.
 */
uintptr_t rsc_ksmachfaultAddress(const RSC_STRUCT_MCONTEXT_L *machineContext);

#if RSC_HAVE_MACH_THREADS
/** Get a thread's thread state and place it in a machine context.
 *
 * @param thread The thread to fetch state for.
 *
 * @param machineContext The machine context to store the state in.
 *
 * @return true if successful.
 */
bool rsc_ksmachthreadState(thread_t thread,
                           RSC_STRUCT_MCONTEXT_L *machineContext);

/** Get a thread's floating point state and place it in a machine context.
 *
 * @param thread The thread to fetch state for.
 *
 * @param machineContext The machine context to store the state in.
 *
 * @return true if successful.
 */
bool rsc_ksmachfloatState(thread_t thread,
                          RSC_STRUCT_MCONTEXT_L *machineContext);

/** Get a thread's exception state and place it in a machine context.
 *
 * @param thread The thread to fetch state for.
 *
 * @param machineContext The machine context to store the state in.
 *
 * @return true if successful.
 */
bool rsc_ksmachexceptionState(thread_t thread,
                              RSC_STRUCT_MCONTEXT_L *machineContext);
#endif

/** Get the number of normal (not floating point or exception) registers the
 *  currently running CPU has.
 *
 * @return The number of registers.
 */
int rsc_ksmachnumRegisters(void);

/** Get the name of a normal register.
 *
 * @param regNumber The register index.
 *
 * @return The register's name or NULL if not found.
 */
const char *rsc_ksmachregisterName(int regNumber);

/** Get the value stored in a normal register.
 *
 * @param regNumber The register index.
 *
 * @return The register's current value.
 */
uint64_t rsc_ksmachregisterValue(const RSC_STRUCT_MCONTEXT_L *machineContext,
                                 int regNumber);

/** Get the number of exception registers the currently running CPU has.
 *
 * @return The number of registers.
 */
int rsc_ksmachnumExceptionRegisters(void);

/** Get the name of an exception register.
 *
 * @param regNumber The register index.
 *
 * @return The register's name or NULL if not found.
 */
const char *rsc_ksmachexceptionRegisterName(int regNumber);

/** Get the value stored in an exception register.
 *
 * @param regNumber The register index.
 *
 * @return The register's current value.
 */
uint64_t
rsc_ksmachexceptionRegisterValue(const RSC_STRUCT_MCONTEXT_L *machineContext,
                                 int regNumber);

/** Get the direction in which the stack grows on the current architecture.
 *
 * @return 1 or -1, depending on which direction the stack grows in.
 */
int rsc_ksmachstackGrowDirection(void);

/** Get a thread's name. Internally, a thread name will
 * never be more than 64 characters long.
 *
 * @param thread The thread whose name to get.
 *
 * @param buffer Buffer to hold the name.
 *
 * @param bufLength The length of the buffer.
 *
 * @return true if a name was found.
 */
bool rsc_ksmachgetThreadName(const thread_t thread, char *const buffer,
                             size_t bufLength);

/** Get the name of a thread's dispatch queue. Internally, a queue name will
 * never be more than 64 characters long.
 *
 * @param thread The thread whose queue name to get.
 *
 * @param buffer Buffer to hold the name.
 *
 * @param bufLength The length of the buffer.
 *
 * @return true if a name or label was found.
 */
bool rsc_ksmachgetThreadQueueName(thread_t thread, char *buffer,
                                  size_t bufLength);

/**
 * Get a thread's current run state.
 *
 * @param thread The thread whose queue name to get.
 *
 * @return The thread's run state or -1 if an error occurred.
 */
integer_t rsc_ksmachgetThreadState(const thread_t thread);

// ============================================================================
#pragma mark - Utility -
// ============================================================================

/* Get the current mach thread ID.
 * mach_thread_self() receives a send right for the thread port which needs to
 * be deallocated to balance the reference count. This function takes care of
 * all of that for you.
 *
 * @return The current thread ID.
 */
thread_t rsc_ksmachthread_self(void);

/** Get a list of all current threads. This list is kernel-allocated, and so must be freed using rsc_ksmachfreeThreads.
 *
 * @param threadCount pointer to location to store the count of all threads.
 *
 * @return A list of threads.
 */
thread_t *rsc_ksmachgetAllThreads(unsigned *threadCount);

/** Free a previously allocated thread list. This should only be called on a thread list allocated from rsc_ksmachgetAllThreads.
 *
 * @param threads The list of threads to free.
 *
 * @param threadCount The number of threads in the list.
 */
void rsc_ksmachfreeThreads(thread_t *threads, unsigned threadCount);

/** Get the run states of a list of threads.
 *
 * @param threads The threads to get states for.
 *
 * @param states A buffer to hold the states.
 *
 * @param threadCount The number of threads in the threads list.
 */
void rsc_ksmachgetThreadStates(thread_t *threads, integer_t *states, unsigned threadCount);

/** Fill out a list containing a source list's contents, with certain threads removed.
 *
 * @param srcThreads The threads list to copy.
 *
 * @param srcThreadCount The number of threads in the source list.
 *
 * @param omitThreads The list of threads to omit when generating the final list.
 *
 * @param omitThreadsCount The number of threads in the omit list.
 *
 * @param dstThreads The destination list to fill out. Must have at least srcThreadCount entries of space.
 *
 * @param maxDstThreads The maximum number of threads that can be stored in dstThreads.
 *
 * @return The number of threads put into the destination list.
 */
unsigned rsc_ksmachremoveThreadsFromList(thread_t *srcThreads, unsigned srcThreadCount,
                                         thread_t *omitThreads, unsigned omitThreadsCount,
                                         thread_t *dstThreads, unsigned maxDstThreads);

#if RSC_HAVE_MACH_THREADS
/** Suspend a list of threads.
 * Note: The current thread cannot be suspended via this function.
 *
 * @param threads The list of threads to suspend.
 *
 * @param threadsCount The number of threads in the list.
 */
void rsc_ksmachsuspendThreads(thread_t *threads, unsigned threadsCount);

/** Resume a list of threads.
 * Note: The current thread cannot be resumed via this function.
 *
 * @param threads The list of threads to suspend.
 *
 * @param threadsCount The number of threads in the list.
 */
void rsc_ksmachresumeThreads(thread_t *threads, unsigned threadsCount);
#endif

/** Copy memory safely. If the memory is not accessible, returns false
 * rather than crashing.
 *
 * @param src The source location to copy from.
 *
 * @param dst The location to copy to.
 *
 * @param numBytes The number of bytes to copy.
 *
 * @return KERN_SUCCESS or an error code.
 */
kern_return_t rsc_ksmachcopyMem(const void *src, void *dst, size_t numBytes);

/** Get the difference in seconds between two timestamps fetched via
 * mach_absolute_time().
 *
 * @param endTime The greater of the two times.
 *
 * @param startTime The lesser of the two times.
 *
 * @return The difference between the two timestamps in seconds.
 */
double rsc_ksmachtimeDifferenceInSeconds(uint64_t endTime, uint64_t startTime);

/** Check if the current process is being traced or not.
 *
 * @return true if we're being traced.
 */
bool rsc_ksmachisBeingTraced(void);

#ifdef __cplusplus
}
#endif

#endif // HDR_KSMach_h
