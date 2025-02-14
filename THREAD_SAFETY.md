# THREAD SAFETY

This document presents the analysis of thread safety of the librpma library.

## Main assumptions

The main assumptions this analysis is based on are following:

1) the API of libibverbs is fully thread-safe and it can be called from every thread in the process (see [Relationship of libibverbs and librdmacm](#relationship-of-libibverbs-and-librdmacm) for details)
2) many threads may use the same peer (`struct rpma_peer`) to create separate connections,
3) there can be only one endpoint (`struct rpma_ep`) and only one thread can use it (call `rpma_ep_next_conn_req()` on it),
4) each of the connections (`struct rpma_conn_req` and `struct rpma_conn`) can be used by only one thread at the same time.

**If the above assumptions are not met, thread safety of the librpma library is not guaranteed.**

The most common scenarios are following:
1) on the active side: the main thread creates connection requests (`struct rpma_conn_req`) for all threads and pass them to those threads which use them to create separate connections (`struct rpma_conn`),
2) on the passive side: the main thread establishes the connection but the rest of work (including connection shutdown) is done by separate thread(s) (if more than one connection is established).

Most of the core librpma API calls are thread-safe but there are also very important exceptions (described below) mainly related to connection's configuration, establishment and tear-down.

Creating resources of RPMA library usually involves dynamic memory allocation and destroying resources usually involves a dynamic memory release. The same resource cannot be destroyed more than once at any thread and a resource cannot be used after it was destroyed. It is the user's responsibility to follow those rules and not doing so may result in a segmentation fault or an undefined behaviour.

## Thread-safe API calls

The following API calls of the librpma library are thread-safe:
- rpma_peer_new
- rpma_peer_delete
- rpma_peer_cfg_new
- rpma_peer_cfg_delete
- rpma_peer_cfg_from_descriptor
- rpma_peer_cfg_get_descriptor_size
- rpma_ep_get_fd
- rpma_conn_cfg_new
- rpma_conn_cfg_delete
- rpma_mr_get_descriptor
- rpma_mr_remote_from_descriptor
- rpma_mr_get_descriptor_size
- rpma_mr_get_ptr
- rpma_mr_get_size
- rpma_mr_remote_get_size
- rpma_mr_remote_delete
- rpma_mr_remote_get_flush_type
- rpma_mr_advise
- rpma_conn_req_get_private_data
- rpma_conn_req_recv
- rpma_conn_delete
- rpma_conn_disconnect
- rpma_conn_get_cq
- rpma_conn_get_compl_fd
- rpma_conn_get_event_fd
- rpma_conn_get_private_data
- rpma_conn_get_qp_num
- rpma_conn_get_rcq
- rpma_conn_next_event
- rpma_conn_wait
- rpma_atomic_write
- rpma_flush
- rpma_read
- rpma_recv
- rpma_send
- rpma_send_with_imm
- rpma_write
- rpma_write_with_imm
- rpma_cq_get_fd
- rpma_cq_wait
- rpma_cq_get_wc
- rpma_utils_ibv_context_is_odp_capable
- rpma_utils_conn_event_2str
- rpma_err_2str
- rpma_log_get_threshold
- rpma_log_set_function
- rpma_log_set_threshold

## Conditionally thread-safe API calls

The following API calls of the librpma library:
- rpma_peer_cfg_set_direct_write_to_pmem
- rpma_peer_cfg_get_direct_write_to_pmem
- rpma_peer_cfg_get_descriptor
- rpma_conn_apply_remote_peer_cfg - calls rpma_peer_cfg_get_direct_write_to_pmem

are thread-safe only if each thread operates on a **separate peer configuration structure** (`struct rpma_peer_cfg`) used only by this one thread. They are not thread-safe if threads operate on one peer configuration structure common for more than one thread.

The following API calls of the librpma library:
- rpma_conn_cfg_get_compl_channel
- rpma_conn_cfg_get_cq_size
- rpma_conn_cfg_get_rcq_size
- rpma_conn_cfg_get_rq_size
- rpma_conn_cfg_get_sq_size
- rpma_conn_cfg_get_timeout
- rpma_conn_cfg_set_compl_channel
- rpma_conn_cfg_set_cq_size
- rpma_conn_cfg_set_rcq_size
- rpma_conn_cfg_set_rq_size
- rpma_conn_cfg_set_sq_size
- rpma_conn_cfg_set_timeout

are thread-safe only if each thread operates on a **separate connection configuration structure** (`struct rpma_conn_cfg`) used only by this one thread. They are not thread-safe if threads operate on one connection configuration structure common for more than one thread.

The following API call of the librpma library:
- rpma_conn_req_connect

is thread-safe only if each thread operates on a **separate connection request** (`struct rpma_conn_req`) used only by this one thread. They are not thread-safe if threads operate on one connection request common for more than one thread.

## NOT thread-safe API calls

The following API calls of the librpma library are NOT thread-safe:
- rpma_conn_req_new
- rpma_conn_req_delete
- rpma_ep_listen
- rpma_ep_next_conn_req
- rpma_ep_shutdown
- rpma_mr_reg
- rpma_mr_dereg
- rpma_utils_get_ibv_context

## Relationship of libibverbs and librdmacm

The API of libibverbs is fully thread-safe and it can be called from every thread in the process.
The detailed description is available at:

 - [rdmamojo/libibverbs](https://www.rdmamojo.com/2013/07/26/libibverbs-thread-safe-level/)
 - [ibv_alloc_td.3](https://man7.org/linux/man-pages/man3/ibv_alloc_td.3.html)

## Analysis of Valgrind suppressions

### Suppressions for libibverbs and librdmacm

The suppressions for libibverbs and librdmacm are described in the [tests/memcheck-libibverbs-librdmacm.supp](tests/memcheck-libibverbs-librdmacm.supp) file.

### Suppressions for the drd tool

The suppressions for the drd tool are described in the [tests/drd.supp](tests/drd.supp) file.

### Suppressions for the helgrind tool

The suppressions for the helgrind tool are described in the [tests/helgrind.supp](tests/helgrind.supp) file.
