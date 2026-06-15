#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Helper to copy memory for later comparison */
static void copy_buffer(const uint8_t *src, uint8_t *dst, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        dst[i] = src[i];
    }
}

/* Helper to compare memory for equality */
static void assert_buffer_eq(const uint8_t *a, const uint8_t *b, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        assert(a[i] == b[i]);
    }
}

/* Helper to assert that a buffer is all zero */
static void assert_buffer_zero(const uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        assert(buf[i] == 0);
    }
}

void aws_secure_zero_harness(void) {
    /* 1. nondeterministic inputs */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    void *pBuf;
    uint8_t *old_contents = NULL;

    /* Choose nondeterministically whether pBuf is NULL */
    if (nondet_bool()) {
        /* pBuf is NULL */
        pBuf = NULL;
    } else {
        /* pBuf is a valid writable buffer (may be zero length) */
        if (bufsize == 0) {
            /* Allocate at least 1 byte to obtain a non‑NULL pointer when size is zero,
               because the function is allowed to receive a non‑NULL pointer with zero size. */
            pBuf = malloc(1);
            __CPROVER_assume(pBuf != NULL);
        } else {
            pBuf = malloc(bufsize);
            __CPROVER_assume(pBuf != NULL);
        }

        /* Fill buffer with nondeterministic data */
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* Save a copy of the original contents for later comparison */
        old_contents = malloc(bufsize);
        __CPROVER_assume(old_contents != NULL || bufsize == 0);
        if (bufsize > 0) {
            copy_buffer(bytes, old_contents, bufsize);
        }
    }

    /* 2. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 3. Post‑conditions */

    if (pBuf == NULL) {
        /* When pBuf is NULL the function asserts that bufsize == 0 */
        assert(bufsize == 0);
        /* No memory to check further */
    } else if (bufsize == 0) {
        /* Zero‑length buffer: function returns early without touching memory */
        uint8_t *bytes = (uint8_t *)pBuf;
        if (old_contents != NULL) {
            assert_buffer_eq(bytes, old_contents, 0);
        }
    } else {
        /* Normal case: buffer must be securely zeroed */
        uint8_t *bytes = (uint8_t *)pBuf;
        assert_buffer_zero(bytes, bufsize);
    }

    /* 4. Clean up */
    if (pBuf != NULL) {
        free(pBuf);
    }
    if (old_contents != NULL) {
        free(old_contents);
    }
}
