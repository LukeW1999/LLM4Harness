#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_secure_zero_harness(void) {
    /* 1. Nondeterministic inputs bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    uint8_t *pBuf = NULL;
    size_t i;

    /* 2. Allocate buffer when size > 0 */
    if (bufsize > 0) {
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL);
        /* initialize with nondet data */
        for (i = 0; i < bufsize; ++i) {
            pBuf[i] = nondet_uint8_t();
        }
    } else {
        /* when size is zero, pBuf may be NULL (any value is fine) */
        pBuf = NULL;
    }

    /* 3. Save old state */
    uint8_t *old_pBuf = pBuf;
    size_t old_bufsize = bufsize;
    struct store_byte_from_buffer old_bytes;
    if (pBuf != NULL && bufsize > 0) {
        save_byte_from_array(pBuf, bufsize, &old_bytes);
    }

    /* 4. Call function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 5. Postconditions */
    /* Unchanged inputs */
    assert(pBuf == old_pBuf);
    assert(bufsize == old_bufsize);

    if (pBuf == NULL || bufsize == 0) {
        /* Early‑return path: must have bufsize == 0 */
        assert(bufsize == 0);
        /* No other observable effect */
    } else {
        /* Success path: buffer must be zeroed */
        for (i = 0; i < bufsize; ++i) {
            assert(pBuf[i] == 0);
        }
    }

    /* 6. Clean up */
    if (pBuf != NULL) {
        free(pBuf);
    }
}
