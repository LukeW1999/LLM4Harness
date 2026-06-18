#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* Declaration of the function under test */
void aws_secure_zero(void *pBuf, size_t bufsize);

void aws_secure_zero_harness(void) {
    /* 1. Non‑deterministic inputs, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    bool make_null = nondet_bool();

    void *pBuf = NULL;
    unsigned char *buf = NULL;          /* convenient typed pointer */
    unsigned char *old_buf = NULL;      /* copy of original contents */

    if (!make_null && bufsize > 0) {
        /* allocate a buffer of the chosen size */
        buf = (unsigned char *)malloc(bufsize);
        __CPROVER_assume(buf != NULL);               /* allocation must succeed for the harness */
        /* initialize buffer with nondet data */
        for (size_t i = 0; i < bufsize; ++i) {
            buf[i] = nondet_uint8_t();
        }
        pBuf = buf;

        /* keep a copy of the original contents for later comparison if needed */
        old_buf = (unsigned char *)malloc(bufsize);
        __CPROVER_assume(old_buf != NULL);
        for (size_t i = 0; i < bufsize; ++i) {
            old_buf[i] = buf[i];
        }
    } else {
        /* pBuf stays NULL or bufsize == 0 */
        pBuf = NULL;
    }

    /* 2. Save immutable inputs */
    size_t old_bufsize = bufsize;
    void *old_pBuf = pBuf;

    /* 3. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 4. Post‑condition checks */

    /* The pointer argument must not be altered */
    assert(pBuf == old_pBuf);
    /* The size argument must not be altered */
    assert(bufsize == old_bufsize);

    if (old_pBuf == NULL || old_bufsize == 0) {
        /* When the buffer is NULL or size is zero the function returns early.
         * No memory is written; we simply ensure we didn't corrupt anything
         * outside the (non‑existent) buffer.  Since we have no observable state,
         * the only thing we can assert is that the inputs remain unchanged,
         * which is already done above. */
    } else {
        /* The buffer must be zeroed byte‑by‑byte */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(buf[i] == 0);
        }
    }

    /* 5. Clean up */
    if (buf != NULL) {
        free(buf);
    }
    if (old_buf != NULL) {
        free(old_buf);
    }
}
