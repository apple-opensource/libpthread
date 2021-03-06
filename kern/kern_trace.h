/*
 * Copyright (c) 2000-2003 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. The rights granted to you under the License
 * may not be used to create, or enable the creation or redistribution of,
 * unlawful or unlicensed copies of an Apple operating system, or to
 * circumvent, violate, or enable the circumvention or violation of, any
 * terms of an Apple operating system software license agreement.
 *
 * Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 */

#ifndef _KERN_TRACE_H_
#define _KERN_TRACE_H_

/* pthread kext, or userspace, kdebug trace points. Defined here and output to
 * /usr/share/misc/pthread.codes during build.
 */

// userspace trace points force slow-paths, so must be compiled in
#define ENABLE_USERSPACE_TRACE 0

// pthread tracing subclasses
# define _TRACE_SUB_DEFAULT 0
# define _TRACE_SUB_WORKQUEUE 1
// WQ_TRACE_REQUESTS_SUBCLASS is 2, in xnu
# define _TRACE_SUB_MUTEX 3
# define _TRACE_SUB_CONDVAR 4
# define _TRACE_SUB_ULMUTEX 5
# define _TRACE_SUB_ULCOND 6

#ifndef _PTHREAD_BUILDING_CODES_

#include <sys/kdebug.h>

#ifndef DBG_PTHREAD
#define DBG_PTHREAD DBG_WORKQUEUE
#endif

#if KERNEL
#include <vm/vm_kern.h>

extern uint32_t pthread_debug_tracing;

static __unused void*
VM_UNSLIDE(void* ptr)
{
    vm_offset_t unslid_ptr;
    vm_kernel_unslide_or_perm_external(ptr, &unslid_ptr);
    return (void*)unslid_ptr;
}

# define PTHREAD_TRACE(x,a,b,c,d) \
	{ if (pthread_debug_tracing) { KERNEL_DEBUG_CONSTANT(TRACE_##x, a, b, c, d, 0); } }

#else // KERNEL

#if ENABLE_USERSPACE_TRACE
# include <sys/kdebug.h>
# define PTHREAD_TRACE(x, a, b, c, d) kdebug_trace(TRACE_##x, a, b, c, d)
#else // ENABLE_USERSPACE_TRACE
# define PTHREAD_TRACE(x, a, b, c, d) do { } while(0)
#endif // ENABLE_USERSPACE_TRACE

#endif // KERNEL

# define TRACE_CODE(name, subclass, code) \
	static const int TRACE_##name = KDBG_CODE(DBG_PTHREAD, subclass, code)

#else // _PTHREAD_BUILDING_CODES_
/* When not included as a header, this file is pre-processed into perl source to generate
 * the pthread.codes file during build.
 */
# define DBG_PTHREAD 9
# define STR(x) #x

# define TRACE_CODE(name, subclass, code) \
	printf("0x%x\t%s\n", ((DBG_PTHREAD << 24) | ((subclass & 0xff) << 16) | ((code & 0x3fff) << 2)), STR(name))
#endif // _PTHREAD_BUILDING_CODES_

/* These defines translate into TRACE_<name> when used in source code, and are
 * pre-processed out to a codes file by the build system.
 */

// "default" trace points
TRACE_CODE(pthread_thread_create, _TRACE_SUB_DEFAULT, 0x10);
TRACE_CODE(pthread_thread_terminate, _TRACE_SUB_DEFAULT, 0x20);
TRACE_CODE(pthread_set_qos_self, _TRACE_SUB_DEFAULT, 0x30);

// synch trace points
TRACE_CODE(psynch_mutex_ulock, _TRACE_SUB_MUTEX, 0x0);
TRACE_CODE(psynch_mutex_utrylock_failed, _TRACE_SUB_MUTEX, 0x1);
TRACE_CODE(psynch_mutex_uunlock, _TRACE_SUB_MUTEX, 0x2);
TRACE_CODE(psynch_ksyn_incorrect_owner, _TRACE_SUB_MUTEX, 0x3);
TRACE_CODE(psynch_mutex_lock_updatebits, _TRACE_SUB_MUTEX, 0x4);
TRACE_CODE(psynch_mutex_unlock_updatebits, _TRACE_SUB_MUTEX, 0x5);
TRACE_CODE(psynch_mutex_clearprepost, _TRACE_SUB_MUTEX, 0x6);
TRACE_CODE(psynch_mutex_kwqallocate, _TRACE_SUB_MUTEX, 0x7);
TRACE_CODE(psynch_mutex_kwqdeallocate, _TRACE_SUB_MUTEX, 0x8);
TRACE_CODE(psynch_mutex_kwqprepost, _TRACE_SUB_MUTEX, 0x9);
TRACE_CODE(psynch_mutex_markprepost, _TRACE_SUB_MUTEX, 0x10);
TRACE_CODE(psynch_mutex_kwqcollision, _TRACE_SUB_MUTEX, 0x11);
TRACE_CODE(psynch_ffmutex_lock_updatebits, _TRACE_SUB_MUTEX, 0x12);
TRACE_CODE(psynch_ffmutex_unlock_updatebits, _TRACE_SUB_MUTEX, 0x13);
TRACE_CODE(psynch_ffmutex_wake, _TRACE_SUB_MUTEX, 0x14);
TRACE_CODE(psynch_mutex_kwqsignal, _TRACE_SUB_MUTEX, 0x15);
TRACE_CODE(psynch_ffmutex_wait, _TRACE_SUB_MUTEX, 0x16);
TRACE_CODE(psynch_mutex_kwqwait, _TRACE_SUB_MUTEX, 0x17);

TRACE_CODE(psynch_cvar_kwait, _TRACE_SUB_CONDVAR, 0x0);
TRACE_CODE(psynch_cvar_clrprepost, _TRACE_SUB_CONDVAR, 0x1);
TRACE_CODE(psynch_cvar_freeitems, _TRACE_SUB_CONDVAR, 0x2);
TRACE_CODE(psynch_cvar_signal, _TRACE_SUB_CONDVAR, 0x3);
TRACE_CODE(psynch_cvar_broadcast, _TRACE_SUB_CONDVAR, 0x5);
TRACE_CODE(psynch_cvar_zeroed, _TRACE_SUB_CONDVAR, 0x6);
TRACE_CODE(psynch_cvar_updateval, _TRACE_SUB_CONDVAR, 0x7);

TRACE_CODE(ulmutex_lock, _TRACE_SUB_ULMUTEX, 0x0);
TRACE_CODE(ulmutex_trylock, _TRACE_SUB_ULMUTEX, 0x1);
TRACE_CODE(ulmutex_lock_wait, _TRACE_SUB_ULMUTEX, 0x2);
TRACE_CODE(ulmutex_unlock, _TRACE_SUB_ULMUTEX, 0x3);
TRACE_CODE(ulmutex_unlock_wake, _TRACE_SUB_ULMUTEX, 0x4);
TRACE_CODE(ulmutex_unlock_steal, _TRACE_SUB_ULMUTEX, 0x5);

TRACE_CODE(ulcond_wait, _TRACE_SUB_ULCOND, 0x0);
TRACE_CODE(ulcond_signal, _TRACE_SUB_ULCOND, 0x1);

#endif // _KERN_TRACE_H_
