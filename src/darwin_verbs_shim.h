/* SPDX-License-Identifier: GPL-2.0 OR BSD-2-Clause */
/*
 * darwin_verbs_shim.h -- fills gaps in Apple's <infiniband/verbs.h>.
 *
 * Apple's RDMA-over-Thunderbolt verbs header (macOS SDK >= 26.2) is a
 * trimmed-down subset of upstream rdma-core: no subnet manager means no
 * multicast groups, no Shared Receive Queue (SRQ) support, no
 * ibv_alloc_null_mr(), and no ibv_alloc_parent_domain() (thread-domain /
 * parent-domain allocation hooks live in verbs_context->ops but are
 * never exposed as a top-level symbol). perftest's -g/--mcg, --use_srq,
 * --null_mr, --no_lock, and --use_cc_unprotected options reference these
 * at compile time even though they're each gated behind a runtime flag
 * that defaults off, so the declarations below exist purely to
 * type-check and link; every entry point fails closed with
 * ENOSYS/EOPNOTSUPP so turning on one of those flags produces a clear
 * runtime error on Darwin instead of a build failure.
 */

#ifndef DARWIN_VERBS_SHIM_H
#define DARWIN_VERBS_SHIM_H

#if defined(__APPLE__)

#include <errno.h>
#include <stdint.h>
#include <infiniband/verbs.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ibv_xrcd;

/* Per-transport ODP capability bits (ibv_odp_caps.per_transport_caps.*);
 * Apple's verbs.h declares the device-wide IBV_ODP_SUPPORT/_IMPLICIT pair
 * but not this enum, even though checking per-transport caps is reachable
 * whenever HAVE_EX_ODP is set (IBV_ACCESS_ON_DEMAND is declared). Values
 * match upstream rdma-core's enum ibv_odp_transport_cap_bits so they stay
 * wire-compatible if Apple ever adds the real bits. */
#ifndef IBV_ODP_SUPPORT_SEND
#define IBV_ODP_SUPPORT_SEND     (1 << 0)
#define IBV_ODP_SUPPORT_RECV     (1 << 1)
#define IBV_ODP_SUPPORT_WRITE    (1 << 2)
#define IBV_ODP_SUPPORT_READ     (1 << 3)
#define IBV_ODP_SUPPORT_ATOMIC   (1 << 4)
#define IBV_ODP_SUPPORT_SRQ_RECV (1 << 5)
#endif

static inline int ibv_attach_mcast(struct ibv_qp *qp, const union ibv_gid *gid, uint16_t lid)
{
	(void)qp; (void)gid; (void)lid;
	errno = ENOSYS;
	return -1;
}
static inline int ibv_detach_mcast(struct ibv_qp *qp, const union ibv_gid *gid, uint16_t lid)
{
	(void)qp; (void)gid; (void)lid;
	errno = ENOSYS;
	return -1;
}

struct ibv_srq_attr {
	uint32_t max_wr;
	uint32_t max_sge;
	uint32_t srq_limit;
};

struct ibv_srq_init_attr {
	void *srq_context;
	struct ibv_srq_attr attr;
};

enum ibv_srq_type {
	IBV_SRQT_BASIC,
	IBV_SRQT_XRC,
	IBV_SRQT_TM,
};

enum ibv_srq_init_attr_mask {
	IBV_SRQ_INIT_ATTR_TYPE = 1 << 0,
	IBV_SRQ_INIT_ATTR_PD = 1 << 1,
	IBV_SRQ_INIT_ATTR_XRCD = 1 << 2,
	IBV_SRQ_INIT_ATTR_CQ = 1 << 3,
};

struct ibv_srq_init_attr_ex {
	void *srq_context;
	struct ibv_srq_attr attr;
	uint32_t comp_mask;
	enum ibv_srq_type srq_type;
	struct ibv_pd *pd;
	struct ibv_xrcd *xrcd;
	struct ibv_cq *cq;
};

struct ibv_srq {
	struct ibv_context *context;
	void *srq_context;
	struct ibv_pd *pd;
	uint32_t handle;
};

static inline struct ibv_srq *ibv_create_srq(struct ibv_pd *pd,
					      struct ibv_srq_init_attr *attr)
{
	(void)pd; (void)attr;
	errno = EOPNOTSUPP;
	return NULL;
}
static inline struct ibv_srq *ibv_create_srq_ex(struct ibv_context *context,
						 struct ibv_srq_init_attr_ex *attr)
{
	(void)context; (void)attr;
	errno = EOPNOTSUPP;
	return NULL;
}
static inline int ibv_destroy_srq(struct ibv_srq *srq)
{
	(void)srq;
	return EOPNOTSUPP;
}
static inline int ibv_get_srq_num(struct ibv_srq *srq, uint32_t *srq_num)
{
	(void)srq; (void)srq_num;
	return EOPNOTSUPP;
}
static inline int ibv_post_srq_recv(struct ibv_srq *srq,
				     struct ibv_recv_wr *recv_wr,
				     struct ibv_recv_wr **bad_recv_wr)
{
	(void)srq; (void)recv_wr;
	*bad_recv_wr = recv_wr;
	return EOPNOTSUPP;
}

static inline struct ibv_mr *ibv_alloc_null_mr(struct ibv_pd *pd)
{
	(void)pd;
	errno = EOPNOTSUPP;
	return NULL;
}

struct ibv_td_init_attr {
	uint32_t comp_mask;
};
struct ibv_td;

struct ibv_parent_domain_init_attr {
	struct ibv_pd *pd;
	struct ibv_td *td;
	uint32_t comp_mask;
	void *(*alloc)(struct ibv_pd *pd, void *pd_context, size_t size,
		       size_t alignment, uint64_t resource_type);
	void (*free)(struct ibv_pd *pd, void *pd_context, void *ptr,
		     uint64_t resource_type);
	void *pd_context;
};

static inline struct ibv_pd *
ibv_alloc_parent_domain(struct ibv_context *context,
			struct ibv_parent_domain_init_attr *attr)
{
	(void)context; (void)attr;
	errno = EOPNOTSUPP;
	return NULL;
}

/* raw_ethernet_resources.h's flow-steering API (used by run_iter_fs() in
 * perftest_resources.c, unconditionally compiled into libperftest.a) needs
 * these declared even though the raw_ethernet_* binaries themselves are
 * never built on Darwin (no subnet manager means no packet classification
 * engine for ibv_create_flow to program). Opaque forward declarations are
 * enough: callers only ever hold pointers to these, never read their
 * fields directly on this platform. */
struct ibv_flow;
struct ibv_flow_attr;

static inline struct ibv_flow *ibv_create_flow(struct ibv_qp *qp,
						struct ibv_flow_attr *flow)
{
	(void)qp; (void)flow;
	errno = EOPNOTSUPP;
	return NULL;
}
static inline int ibv_destroy_flow(struct ibv_flow *flow_id)
{
	(void)flow_id;
	return EOPNOTSUPP;
}

#ifdef __cplusplus
}
#endif

#endif /* __APPLE__ */

#endif /* DARWIN_VERBS_SHIM_H */
