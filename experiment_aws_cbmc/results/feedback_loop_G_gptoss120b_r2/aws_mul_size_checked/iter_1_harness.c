#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_secure_zero_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    void *pBuf;
    bool is_null = nondet_bool();

    if (is_null) {
        pBuf = NULL;
        /* Precondition: if pBuf is NULL then bufsize must be 0 (asserted inside the function) */
        __CPROVER_assume(bufsize == 0);
    } else {
        if (bufsize == 0) {
            pBuf = NULL; /* malloc(0) may return NULL; treat as NULL for simplicity */
        } else {
            pBuf = malloc(bufsize);
            __CPROVER_assume(pBuf != NULL);
            /* Fill the buffer with nondet data */
            uint8_t *bytes = (uint8_t *)pBuf;
            for (size_t i = 0; i < bufsize; ++i) {
                bytes[i] = nondet_uint8_t();
            }
        }
    }

    /* 2. Save old state */
    void *old_pBuf = pBuf;
    size_t old_bufsize = bufsize;

    /* 3. Call function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 4. Post‑conditions */

    /* The pointer and size arguments must remain unchanged */
    assert(pBuf == old_pBuf);
    assert(bufsize == old_bufsize);

    if (pBuf == NULL) {
        /* When the buffer is NULL the function returns early; bufsize must be 0 */
        assert(bufsize == 0);
    } else {
        /* All bytes in the buffer must be zero after the call */
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            assert(bytes[i] == 0);
        }
    }

    /* 5. Clean up */
    if (pBuf != NULL) {
        free(pBuf);
    }
}
