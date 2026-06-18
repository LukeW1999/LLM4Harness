#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* MAX_BUFFER_SIZE is defined by the proof harness Makefile */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void aws_secure_zero_harness(void) {
    /* nondeterministic size, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* nondeterministically decide whether the buffer pointer is NULL */
    bool ptr_is_null = nondet_bool();

    void *pBuf = NULL;
    uint8_t *old_contents = NULL;

    if (ptr_is_null) {
        pBuf = NULL;
        /* The implementation asserts that bufsize == 0 when pBuf is NULL */
        __CPROVER_assume(bufsize == 0);
    } else {
        /* allocate a buffer large enough for the maximum size */
        pBuf = malloc(MAX_BUFFER_SIZE);
        __CPROVER_assume(pBuf != NULL);

        /* fill the entire allocated region with nondeterministic bytes */
        uint8_t *buf = (uint8_t *)pBuf;
        for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
            buf[i] = nondet_uint8_t();
        }

        /* save a copy of the original contents for later comparison */
        old_contents = malloc(MAX_BUFFER_SIZE);
        __CPROVER_assume(old_contents != NULL);
        for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
            old_contents[i] = buf[i];
        }
    }

    /* call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* post‑conditions */
    if (pBuf != NULL) {
        uint8_t *buf = (uint8_t *)pBuf;

        /* bytes within the requested range must be zeroed */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(buf[i] == 0);
        }

        /* bytes outside the requested range must be unchanged */
        for (size_t i = bufsize; i < MAX_BUFFER_SIZE; ++i) {
            assert(buf[i] == old_contents[i]);
        }
    } else {
        /* when pBuf is NULL the function does nothing else; no state to check */
    }

    /* clean up */
    if (pBuf != NULL) {
        free(pBuf);
    }
    if (old_contents != NULL) {
        free(old_contents);
    }
}
