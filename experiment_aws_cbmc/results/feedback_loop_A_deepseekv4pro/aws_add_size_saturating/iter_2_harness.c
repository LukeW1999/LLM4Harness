/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

/* Maximum buffer size to limit state space */
#define MAX_BUF_SIZE 1024

void aws_secure_zero_harness(void) {
    /* 1. Declare and bound parameters */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUF_SIZE);

    uint8_t *pBuf;
    if (bufsize > 0) {
        pBuf = (uint8_t *)malloc(bufsize);
        /* Assume memory allocation succeeds if we request it */
        __CPROVER_assume(pBuf != NULL);
        /* Fill buffer with nondeterministic bytes */
        for (size_t i = 0; i < bufsize; i++) {
            pBuf[i] = nondet_uint8_t();
        }
    } else {
        pBuf = NULL;
    }

    /* Enforce the function's documented precondition:
     *   If pBuf is NULL then bufsize must be 0.
     * This avoids triggering the AWS_ASSERT inside aws_secure_zero.
     */
    __CPROVER_assume(pBuf != NULL || bufsize == 0);

    /* 2. Save old state (buffer contents) */
    uint8_t *old = NULL;
    if (bufsize > 0) {
        old = (uint8_t *)malloc(bufsize);
        __CPROVER_assume(old != NULL);
        memcpy(old, pBuf, bufsize);
    }

    /* 3. Call function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 4. Assert postconditions */
    /* If buffer is non‑NULL and size > 0, all bytes must be zeroed */
    if (pBuf != NULL && bufsize > 0) {
        for (size_t i = 0; i < bufsize; i++) {
            assert(pBuf[i] == 0);
        }
    }

    /* Pointer value itself must not change (NULL stays NULL, non‑NULL stays the same) */
    /* (Not strictly needed since pBuf is local, but demonstrates invariance) */
    if (bufsize > 0) {
        assert(pBuf != NULL);   /* it was non‑NULL initially, remains valid */
    } else {
        assert(pBuf == NULL);   /* empty buffer case */
    }

    /* Cleanup */
    if (old != NULL) {
        free(old);
    }
    if (pBuf != NULL) {
        free(pBuf);
    }
}
