/* SPDX-License-Identifier: GPL-2.0 OR BSD-2-Clause */
/*
 * darwin_rdma_cm_shim.h -- stand-in for <rdma/rdma_cma.h> on Darwin.
 *
 * Apple's RDMA-over-Thunderbolt stack (librdma.dylib, macOS SDK >= 26.2)
 * only exports the base ibverbs control-plane calls (see
 * mlx/mlx/distributed/jaccl/lib/jaccl/rdma.cpp for the exact symbol list
 * JACCL loads) -- there is no librdmacm equivalent. perftest only needs
 * rdma_cm for its optional -R/--rdma_cm and -z/--use_rdma_cm connection
 * managers; the default path (used by plain `-c UC` bw/lat runs) does its
 * own socket handshake and calls ibv_modify_qp() directly, bypassing all of
 * this. These declarations exist purely so perftest_resources.h /
 * perftest_communication.h / perftest_communication.c type-check and link
 * on Darwin; every function fails closed with ENOSYS so -R/-z produce a
 * clear error instead of a silent no-op.
 */

#ifndef DARWIN_RDMA_CM_SHIM_H
#define DARWIN_RDMA_CM_SHIM_H

#if defined(__APPLE__)

#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <infiniband/verbs.h>

#ifdef __cplusplus
extern "C" {
#endif

enum rdma_cm_event_type {
	RDMA_CM_EVENT_ADDR_RESOLVED,
	RDMA_CM_EVENT_ADDR_ERROR,
	RDMA_CM_EVENT_ROUTE_RESOLVED,
	RDMA_CM_EVENT_ROUTE_ERROR,
	RDMA_CM_EVENT_CONNECT_REQUEST,
	RDMA_CM_EVENT_CONNECT_RESPONSE,
	RDMA_CM_EVENT_CONNECT_ERROR,
	RDMA_CM_EVENT_UNREACHABLE,
	RDMA_CM_EVENT_REJECTED,
	RDMA_CM_EVENT_ESTABLISHED,
	RDMA_CM_EVENT_DISCONNECTED,
	RDMA_CM_EVENT_DEVICE_REMOVAL,
	RDMA_CM_EVENT_MULTICAST_JOIN,
	RDMA_CM_EVENT_MULTICAST_ERROR,
	RDMA_CM_EVENT_ADDR_CHANGE,
	RDMA_CM_EVENT_TIMEWAIT_EXIT,
};

enum rdma_port_space {
	RDMA_PS_TCP = 0x0106,
	RDMA_PS_UDP = 0x0111,
};

struct rdma_event_channel {
	int fd;
};

struct rdma_route {
	struct {
		struct sockaddr_storage src_storage;
		struct sockaddr_storage dst_storage;
	} addr;
};

struct rdma_cm_id {
	struct ibv_context	*verbs;
	struct rdma_event_channel *channel;
	void			*context;
	struct ibv_qp		*qp;
	struct rdma_route	 route;
	enum rdma_port_space	 ps;
	uint8_t			 port_num;
};

struct rdma_conn_param {
	const void *private_data;
	uint8_t private_data_len;
	uint8_t responder_resources;
	uint8_t initiator_depth;
	uint8_t flow_control;
	uint8_t retry_count;
	uint8_t rnr_retry_count;
	uint8_t srq;
	uint32_t qp_num;
};

struct rdma_ud_param {
	const void *private_data;
	uint8_t private_data_len;
	struct ibv_ah_attr ah_attr;
	uint32_t qp_num;
	uint32_t qkey;
};

struct rdma_cm_event {
	struct rdma_cm_id	*id;
	struct rdma_cm_id	*listen_id;
	enum rdma_cm_event_type	 event;
	int			 status;
	union {
		struct rdma_conn_param conn;
		struct rdma_ud_param   ud;
		uint64_t arg;
	} param;
};

struct rdma_addrinfo {
	int			ai_flags;
	int			ai_family;
	int			ai_qp_type;
	int			ai_port_space;
	socklen_t		ai_src_len;
	socklen_t		ai_dst_len;
	struct sockaddr		*ai_src_addr;
	struct sockaddr		*ai_dst_addr;
	char			*ai_src_canonname;
	char			*ai_dst_canonname;
	size_t			ai_route_len;
	void			*ai_route;
	size_t			ai_connect_len;
	void			*ai_connect;
	struct rdma_addrinfo	*ai_next;
};

#define RAI_PASSIVE		0x00000001
#define RAI_NUMERICHOST		0x00000002
#define RAI_NOROUTE		0x00000004
#define RAI_FAMILY		0x00000008

enum {
	RDMA_OPTION_ID = 0,
};

enum {
	RDMA_OPTION_ID_TOS = 0,
	RDMA_OPTION_ID_ACK_TIMEOUT = 3,
};

static inline struct rdma_event_channel *rdma_create_event_channel(void)
{
	errno = ENOSYS;
	return NULL;
}
static inline void rdma_destroy_event_channel(struct rdma_event_channel *c) { (void)c; }
static inline int rdma_create_id(struct rdma_event_channel *c, struct rdma_cm_id **id,
				  void *context, enum rdma_port_space ps)
{
	(void)c; (void)id; (void)context; (void)ps;
	errno = ENOSYS;
	return -1;
}
static inline int rdma_destroy_id(struct rdma_cm_id *id) { (void)id; return 0; }
static inline int rdma_bind_addr(struct rdma_cm_id *id, struct sockaddr *addr)
{
	(void)id; (void)addr;
	errno = ENOSYS;
	return -1;
}
static inline int rdma_resolve_addr(struct rdma_cm_id *id, struct sockaddr *src,
				     struct sockaddr *dst, int timeout_ms)
{
	(void)id; (void)src; (void)dst; (void)timeout_ms;
	errno = ENOSYS;
	return -1;
}
static inline int rdma_resolve_route(struct rdma_cm_id *id, int timeout_ms)
{
	(void)id; (void)timeout_ms;
	errno = ENOSYS;
	return -1;
}
static inline int rdma_create_qp(struct rdma_cm_id *id, struct ibv_pd *pd,
				  struct ibv_qp_init_attr *attr)
{
	(void)id; (void)pd; (void)attr;
	errno = ENOSYS;
	return -1;
}
/* Apple's trimmed infiniband/verbs.h has no extended QP attr API (JACCL's
 * rdma.cpp only ever calls the base ibv_create_qp()); forward-declare so
 * this always-failing stub still type-checks without a visibility warning. */
struct ibv_qp_init_attr_ex;
static inline int rdma_create_qp_ex(struct rdma_cm_id *id,
				     struct ibv_qp_init_attr_ex *attr)
{
	(void)id; (void)attr;
	errno = ENOSYS;
	return -1;
}
static inline void rdma_destroy_qp(struct rdma_cm_id *id) { (void)id; }
static inline int rdma_connect(struct rdma_cm_id *id, struct rdma_conn_param *p)
{
	(void)id; (void)p;
	errno = ENOSYS;
	return -1;
}
static inline int rdma_listen(struct rdma_cm_id *id, int backlog)
{
	(void)id; (void)backlog;
	errno = ENOSYS;
	return -1;
}
static inline int rdma_accept(struct rdma_cm_id *id, struct rdma_conn_param *p)
{
	(void)id; (void)p;
	errno = ENOSYS;
	return -1;
}
static inline int rdma_reject(struct rdma_cm_id *id, const void *pd, uint8_t pdl)
{
	(void)id; (void)pd; (void)pdl;
	errno = ENOSYS;
	return -1;
}
static inline int rdma_disconnect(struct rdma_cm_id *id)
{
	(void)id;
	errno = ENOSYS;
	return -1;
}
static inline int rdma_get_cm_event(struct rdma_event_channel *c,
				     struct rdma_cm_event **event)
{
	(void)c; (void)event;
	errno = ENOSYS;
	return -1;
}
static inline int rdma_ack_cm_event(struct rdma_cm_event *event)
{
	(void)event;
	return 0;
}
static inline struct sockaddr *rdma_get_local_addr(struct rdma_cm_id *id)
{
	return (struct sockaddr *)&id->route.addr.src_storage;
}
static inline const char *rdma_event_str(enum rdma_cm_event_type event)
{
	(void)event;
	return "rdma_cm unsupported on Darwin";
}
static inline int rdma_set_option(struct rdma_cm_id *id, int level, int optname,
				   void *optval, size_t optlen)
{
	(void)id; (void)level; (void)optname; (void)optval; (void)optlen;
	errno = ENOSYS;
	return -1;
}
static inline int rdma_getaddrinfo(const char *node, const char *service,
				    struct rdma_addrinfo *hints,
				    struct rdma_addrinfo **res)
{
	(void)node; (void)service; (void)hints; (void)res;
	errno = ENOSYS;
	return EAI_FAIL;
}
static inline void rdma_freeaddrinfo(struct rdma_addrinfo *res) { (void)res; }

#ifdef __cplusplus
}
#endif

#endif /* __APPLE__ */

#endif /* DARWIN_RDMA_CM_SHIM_H */
