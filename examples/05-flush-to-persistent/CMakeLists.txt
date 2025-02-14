#
# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2020-2022, Intel Corporation
# Copyright 2021, Fujitsu
#

cmake_minimum_required(VERSION 3.3)
project(flush-to-persistent C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	${CMAKE_SOURCE_DIR}/../cmake
	${CMAKE_SOURCE_DIR}/../../cmake)

include(${CMAKE_SOURCE_DIR}/../cmake/common.cmake)
include(${CMAKE_SOURCE_DIR}/../../cmake/functions.cmake)

# set LIBRT_LIBRARIES if linking with librt is required
check_if_librt_is_required()

set(LIBPMEM_REQUIRED_VERSION 1.6)

find_package(PkgConfig QUIET)

if(PKG_CONFIG_FOUND)
	pkg_check_modules(LIBRPMA librpma)
	pkg_check_modules(LIBIBVERBS libibverbs)
	pkg_check_modules(LIBPMEM QUIET libpmem>=${LIBPMEM_REQUIRED_VERSION})
endif()
if(NOT LIBRPMA_FOUND)
	find_package(LIBRPMA REQUIRED librpma)
endif()
if(NOT LIBIBVERBS_FOUND)
	find_package(LIBIBVERBS REQUIRED libibverbs)
endif()
if(NOT LIBPMEM_FOUND)
	find_package(LIBPMEM ${LIBPMEM_REQUIRED_VERSION} QUIET)
endif()

link_directories(${LIBRPMA_LIBRARY_DIRS})

add_example_with_pmem(server server.c ../common/common-conn.c)
add_example_with_pmem(client client.c ../common/common-conn.c)
