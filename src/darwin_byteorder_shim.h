/* SPDX-License-Identifier: GPL-2.0 OR BSD-2-Clause */
/*
 * darwin_byteorder_shim.h -- stand-in for <infiniband/byteorder.h> on Darwin.
 *
 * raw_ethernet_resources.h's on-wire packet structs (IPv4/IPv6/UDP headers)
 * use kernel-style fixed-width typedefs (__u8, __be16, ...) that Linux gets
 * from <linux/types.h> (pulled in transitively via infiniband/verbs.h) and
 * FreeBSD provides via its own <infiniband/byteorder.h>. Apple's trimmed
 * verbs.h has neither. raw_ethernet_resources.h is included unconditionally
 * from perftest_parameters.c (not gated behind HAVE_RAW_ETH), so these
 * typedefs must exist even though the raw_ethernet_* binaries themselves
 * won't be built on Darwin (ibv_create_flow is not declared there either).
 */

#ifndef DARWIN_BYTEORDER_SHIM_H
#define DARWIN_BYTEORDER_SHIM_H

#if defined(__APPLE__)

#include <stdint.h>

typedef uint8_t  __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;
typedef int8_t   __s8;
typedef int16_t  __s16;
typedef int32_t  __s32;
typedef int64_t  __s64;

typedef uint16_t __be16;
typedef uint32_t __be32;
typedef uint64_t __be64;
typedef uint16_t __le16;
typedef uint32_t __le32;
typedef uint64_t __le64;

#endif /* __APPLE__ */

#endif /* DARWIN_BYTEORDER_SHIM_H */
