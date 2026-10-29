/* SPDX-License-Identifier: GPL-2.0 OR BSD-2-Clause */
/*
 * darwin_umad_shim.h -- stand-in for <infiniband/umad.h> on Darwin.
 *
 * libibumad talks to the IB subnet manager to join multicast groups
 * (perftest's -g/--mcg option, used only by ib_send_bw/ib_send_lat). Apple's
 * RDMA-over-Thunderbolt fabric is a point-to-point link with no subnet
 * manager, so there is no multicast/SA concept and no umad library to link
 * against. ib_send_bw/lat call join_multicast_group() unconditionally from
 * multicast_resources.c, but only actually run it when the user passes -g;
 * a plain `-c UC` bw/lat run never reaches it. This shim exists so that
 * code type-checks and links; every entry point fails closed with ENOSYS.
 */

#ifndef DARWIN_UMAD_SHIM_H
#define DARWIN_UMAD_SHIM_H

#if defined(__APPLE__)

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline int umad_init(void) { errno = ENOSYS; return -1; }
static inline int umad_open_port(const char *ca_name, int portnum)
{
	(void)ca_name; (void)portnum;
	errno = ENOSYS;
	return -1;
}
static inline int umad_close_port(int portid) { (void)portid; return 0; }
static inline int umad_register(int portid, int mgmt_class, int mgmt_version,
				 uint8_t rmpp_version, long method_mask[16 / sizeof(long)])
{
	(void)portid; (void)mgmt_class; (void)mgmt_version;
	(void)rmpp_version; (void)method_mask;
	errno = ENOSYS;
	return -1;
}
static inline int umad_unregister(int portid, int agentid)
{
	(void)portid; (void)agentid;
	return 0;
}
static inline void *umad_alloc(int num, size_t size)
{
	(void)num; (void)size;
	errno = ENOSYS;
	return NULL;
}
static inline void umad_free(void *umad) { (void)umad; }
static inline void *umad_get_mad(void *umad) { (void)umad; return NULL; }
static inline size_t umad_size(void) { return 0; }
static inline int umad_set_addr(void *umad, int dlid, int dqp, int sl, int qkey)
{
	(void)umad; (void)dlid; (void)dqp; (void)sl; (void)qkey;
	errno = ENOSYS;
	return -1;
}
static inline int umad_set_pkey(void *umad, int pkey_index)
{
	(void)umad; (void)pkey_index;
	errno = ENOSYS;
	return -1;
}
static inline int umad_send(int portid, int agentid, void *umad, int length,
			     int timeout_ms, int retries)
{
	(void)portid; (void)agentid; (void)umad; (void)length;
	(void)timeout_ms; (void)retries;
	errno = ENOSYS;
	return -1;
}
static inline int umad_recv(int portid, void *umad, int *length, int timeout_ms)
{
	(void)portid; (void)umad; (void)length; (void)timeout_ms;
	errno = ENOSYS;
	return -1;
}

#ifdef __cplusplus
}
#endif

#endif /* __APPLE__ */

#endif /* DARWIN_UMAD_SHIM_H */
