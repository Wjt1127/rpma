// SPDX-License-Identifier: BSD-3-Clause
/* Copyright 2022, Intel Corporation */

/*
 * rpma_mr_dereg.c -- rpma_mr_dereg multithreaded test
 */

#include <stdlib.h>
#include <librpma.h>

#include "mtt.h"

struct prestate {
	char *addr;
	unsigned port;
	struct ibv_context *ibv_ctx;
	struct rpma_peer *peer;
};

struct state {
	void *mr_ptr;
	struct rpma_mr_local *mr;
};

/*
 * prestate_init -- obtain an ibv_context for a local IP address
 * and create a new peer object
 */
static void
prestate_init(void *prestate, struct mtt_result *tr)
{
	struct prestate *pr = (struct prestate *)prestate;
	int ret;

	if ((ret = rpma_utils_get_ibv_context(pr->addr,
			RPMA_UTIL_IBV_CONTEXT_LOCAL, &pr->ibv_ctx))) {
		MTT_RPMA_ERR(tr, "rpma_utils_get_ibv_context", ret);
		return;
	}

	if ((ret = rpma_peer_new(pr->ibv_ctx, &pr->peer)))
		MTT_RPMA_ERR(tr, "rpma_peer_new", ret);
}

/*
 * seq_init -- allocate state, memory region and register the memory
 */
void
seq_init(unsigned id, void *prestate, void **state_ptr, struct mtt_result *result)
{
	struct prestate *pr = (struct prestate *)prestate;
	struct state *st = (struct state *)calloc(1, sizeof(struct state));
	if (!st) {
		MTT_ERR(result, "calloc", errno);
		return;
	}

	st->mr_ptr = mtt_malloc_aligned(KILOBYTE, result);
	if (st->mr_ptr == NULL) {
		free(st);
		return;
	}

	*state_ptr = st;

	int ret = rpma_mr_reg(pr->peer, st->mr_ptr, KILOBYTE, RPMA_MR_USAGE_READ_SRC, &st->mr);
	if (ret)
		MTT_RPMA_ERR(result, "rpma_mr_reg", ret);
}

/*
 * thread -- deregister the memory region
 */
static void
thread(unsigned id, void *prestate, void *state, struct mtt_result *result)
{
	struct state *st = (struct state *)state;
	int ret;

	if (st->mr) {
		if ((ret = rpma_mr_dereg(&st->mr)))
			MTT_RPMA_ERR(result, "rpma_mr_dereg", ret);
	}
}

/*
 * seq_fini -- free the state
 */
static void
seq_fini(unsigned id, void *prestate, void **state_ptr, struct mtt_result *tr)
{
	struct state *st = (struct state *)*state_ptr;

	free(st->mr_ptr);
	free(st);
	*state_ptr = NULL;
}

/*
 * prestate_fini -- delete the peer object
 */
static void
prestate_fini(void *prestate, struct mtt_result *tr)
{
	struct prestate *pr = (struct prestate *)prestate;
	int ret;

	if ((ret = rpma_peer_delete(&pr->peer)))
		MTT_RPMA_ERR(tr, "rpma_peer_delete", ret);
}

int
main(int argc, char *argv[])
{
	struct mtt_args args = {0};

	if (mtt_parse_args(argc, argv, &args))
		return -1;

	struct prestate prestate = {args.addr, args.port, NULL, NULL};

	struct mtt_test test = {
			&prestate,
			prestate_init,
			seq_init,
			NULL,
			thread,
			NULL,
			seq_fini,
			prestate_fini
	};

	return mtt_run(&test, args.threads_num);
}
