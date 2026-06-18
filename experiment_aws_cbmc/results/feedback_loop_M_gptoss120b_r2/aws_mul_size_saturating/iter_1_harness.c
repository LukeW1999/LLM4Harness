#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_secure_zero_harness(void) {
    /* Non-deterministic inputs */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    bool make_null = nondet_bool();

    void *pBuf;
    if (make_null) {
        pBuf = NULL;
    } else {
        /* Allocate a buffer of the requested size */
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL);
    }

    /* Save old state */
    void *old_pBuf = pBuf;
    size_t old_bufsize = bufsize;

    /* Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* Postconditions */
    if (pBuf == NULL || bufsize == 0) {
        /* Early‑return path: the function asserts bufsize == 0 */
        assert(bufsize == 0);
        /* No modifications to inputs */
        assert(pBuf == old_pBuf);
        assert(bufsize == old_bufsize);
    } else {
        /* Zeroing path: all bytes must be set to 0 */
        uint8_t *bytes = (uint8_t *)pBuf;
        size_t i;
        for (i = 0; i < bufsize; ++i) {
            assert(bytes[i] == 0);
        }
        /* Inputs remain unchanged */
        assert(pBuf == old_pBuf);
        assert(bufsize == old_bufsize);
    }
}
