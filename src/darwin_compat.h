/* SPDX-License-Identifier: GPL-2.0 OR BSD-2-Clause */
/*
 * darwin_compat.h -- glibc-only primitives perftest assumes unconditionally
 * (byteswap.h macros, Linux cpu_set_t) that have no equivalent in the macOS
 * SDK. Included only when building on Darwin, in place of <malloc.h> /
 * <byteswap.h> / <sched.h>'s CPU_* macros.
 *
 * CPU affinity is a best-effort optimization (--pin_cores); it is not
 * required for a UC bw/lat run over RDMA-over-Thunderbolt, so it is a
 * harmless no-op here rather than wired to a Mach thread_policy_set shim.
 */

#ifndef DARWIN_COMPAT_H
#define DARWIN_COMPAT_H

#if defined(__APPLE__)

#include <libkern/OSByteOrder.h>

/* perftest_resources.c uses the Linux-kernel-style __always_inline macro
 * (FreeBSD's <sys/cdefs.h> defines it; glibc pulls it in transitively).
 * Apple's <sys/cdefs.h> does not, so define it directly. */
#ifndef __always_inline
#define __always_inline __attribute__((__always_inline__))
#endif

#ifndef htobe16
#define htobe16(x) OSSwapHostToBigInt16(x)
#endif
#ifndef htobe32
#define htobe32(x) OSSwapHostToBigInt32(x)
#endif
#ifndef htobe64
#define htobe64(x) OSSwapHostToBigInt64(x)
#endif
#ifndef be16toh
#define be16toh(x) OSSwapBigToHostInt16(x)
#endif
#ifndef be32toh
#define be32toh(x) OSSwapBigToHostInt32(x)
#endif
#ifndef be64toh
#define be64toh(x) OSSwapBigToHostInt64(x)
#endif
#ifndef htole16
#define htole16(x) OSSwapHostToLittleInt16(x)
#endif
#ifndef htole32
#define htole32(x) OSSwapHostToLittleInt32(x)
#endif
#ifndef htole64
#define htole64(x) OSSwapHostToLittleInt64(x)
#endif
#ifndef le16toh
#define le16toh(x) OSSwapLittleToHostInt16(x)
#endif
#ifndef le32toh
#define le32toh(x) OSSwapLittleToHostInt32(x)
#endif
#ifndef le64toh
#define le64toh(x) OSSwapLittleToHostInt64(x)
#endif
#ifndef bswap_16
#define bswap_16(x) OSSwapInt16(x)
#endif
#ifndef bswap_32
#define bswap_32(x) OSSwapInt32(x)
#endif
#ifndef bswap_64
#define bswap_64(x) OSSwapInt64(x)
#endif

/* perftest_parameters.h declares a cpu_set_t field unconditionally.
 * Provide a minimal fixed-width stand-in; set_process_affinity() treats
 * CPU_COUNT() == 0 as "no explicit --pin_cores request" and falls back to
 * NUMA/no-op, so this only needs to support the parse + count path. */
#ifndef DARWIN_CPU_SETSIZE
#define DARWIN_CPU_SETSIZE 64
#endif

typedef struct {
	unsigned long bits;
} cpu_set_t;

#define CPU_ZERO(set) ((void)((set)->bits = 0))
#define CPU_SET(cpu, set) \
	((void)((set)->bits |= (1UL << ((cpu) % DARWIN_CPU_SETSIZE))))
#define CPU_CLR(cpu, set) \
	((void)((set)->bits &= ~(1UL << ((cpu) % DARWIN_CPU_SETSIZE))))
#define CPU_ISSET(cpu, set) \
	(((set)->bits & (1UL << ((cpu) % DARWIN_CPU_SETSIZE))) != 0)
#define CPU_COUNT(set) (__builtin_popcountl((set)->bits))

/* No portable equivalent to sched_setaffinity()/sched_getcpu() on macOS
 * (would require Mach thread_policy_set with THREAD_AFFINITY_POLICY, which
 * is only a hint on Apple Silicon's AMP scheduler). Call sites are guarded
 * with __linux__ instead of relying on these; kept here only so code that
 * merely references the symbols still links. */
static inline int sched_setaffinity(int pid, size_t cpusetsize, const cpu_set_t *mask)
{
	(void)pid; (void)cpusetsize; (void)mask;
	return 0;
}

static inline int sched_getcpu(void)
{
	return 0;
}

/* Always fails so the "shrink NUMA binding to an affinity-restricted cpuset"
 * heuristic in perftest_parameters.c is skipped rather than fed bogus data. */
static inline int sched_getaffinity(int pid, size_t cpusetsize, cpu_set_t *mask)
{
	(void)pid; (void)cpusetsize; (void)mask;
	return -1;
}

#endif /* __APPLE__ */

#endif /* DARWIN_COMPAT_H */
