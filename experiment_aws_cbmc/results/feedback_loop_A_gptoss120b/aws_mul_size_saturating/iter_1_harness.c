#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "aws/common/common.h"

/* Proof helper declarations (normally provided by the proof harness framework) */
bool nondet_bool(void);
size_t nondet_size_t(void);
void __CPROVER_assume(_Bool);
void assert(_Bool);

/* Maximum size for the buffer to keep the state space bounded */
#ifndef MAX_BUFFER_SIZE
#   define MAX_BUFFER_SIZE 64
#endif

void aws_secure_zero_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    bool make_null = nondet_bool();

    void *pBuf;
    void *old_pBuf;
    size_t old_bufsize = bufsize;

    if (make_null) {
        /* When the pointer is NULL the function asserts that bufsize == 0 */
        __CPROVER_assume(bufsize == 0);
        pBuf = NULL;
    } else {
        /* Allocate a buffer of the chosen size */
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL);
    }

    old_pBuf = pBuf; /* remember the original pointer */

    /* 2. Save old memory contents (if any) */
    uint8_t *old_bytes = NULL;
    if (pBuf != NULL && bufsize > 0) {
        old_bytes = malloc(bufsize);
        __CPROVER_assume(old_bytes != NULL);
        memcpy(old_bytes, pBuf, bufsize);
    }

    /* 3. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 4. Post‑condition checks */

    /* The pointer itself must not change */
    assert(pBuf == old_pBuf);
    /* The size argument must not change */
    assert(bufsize == old_bufsize);

    if (pBuf != NULL && bufsize > 0) {
        /* On success the whole region must be zeroed */
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            assert(bytes[i] == 0);
        }
    } else {
        /* When no zeroing occurs the memory must be unchanged */
        if (old_bytes != NULL) {
            uint8_t *bytes = (uint8_t *)pBuf;
            for (size_t i = 0; i < bufsize; ++i) {
                assert(bytes[i] == old_bytes[i]);
            }
        }
    }

    /* 5. Clean up (not required for verification but keeps tools happy) */
    if (pBuf != NULL) {
        free(pBuf);
    }
    if (old_bytes != NULL) {
        free(old_bytes);
    }
}
