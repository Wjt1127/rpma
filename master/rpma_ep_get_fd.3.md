---
layout: manual
Content-Style: 'text/css'
title: LIBRPMA
collection: librpma
date: rpma API version 0.0
...

[comment]: <> (SPDX-License-Identifier: BSD-3-Clause)
[comment]: <> (Copyright 2020, Intel Corporation)

NAME
====

**rpma\_ep\_get\_fd** - get a file descriptor of the endpoint

SYNOPSIS
========

          #include <librpma.h>

          int rpma_ep_get_fd(struct rpma_ep *ep, int *fd);

DESCRIPTION
===========

Get a file descriptor of the endpoint.

ERRORS
======

**rpma\_ep\_get\_fd**() can fail with the following error:

-   RPMA\_E\_INVAL - *ep* or *fd* is NULL