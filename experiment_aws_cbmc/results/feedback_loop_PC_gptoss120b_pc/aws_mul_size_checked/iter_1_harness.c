#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_secure_zero_harness(void) {
    /* nondeterministic inputs */
    size_t bufsize = nondet_size_t();
    bool make_null = nondet_bool();

    /* bound the size to keep state space reasonable */
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    void *pBuf;
    void *old_pBuf;
    size_t old_bufsize = bufsize;

    if (make_null) {
        pBuf = NULL;
    } else {
        /* allocate a buffer of the requested size */
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL);
    }

    old_pBuf = pBuf;

    /* precondition: if pBuf is NULL then bufsize must be 0 (matches the internal assert) */
    __CPROVER_assume(!(pBuf == NULL && bufsize != 0));

    /* Save a copy of the original buffer contents for later comparison */
    uint8_t *old_contents = NULL;
    if (pBuf != NULL && bufsize > 0) {
        old_contents = malloc(bufsize);
        __CPROVER_assume(old_contents != NULL);
        memcpy(old_contents, pBuf, bufsize);
    }

    /* Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* Postconditions */

    /* The pointer argument itself must not change */
    assert(pBuf == old_pBuf);

    /* The size argument must not change */
    assert(bufsize == old_bufsize);

    if (pBuf != NULL) {
        uint8_t *buf = (uint8_t *)pBuf;
        if (bufsize > 0) {
            /* When size > 0 the buffer must be zeroed */
            for (size_t i = 0; i < bufsize; ++i) {
                assert(buf[i] == 0);
            }
        } else {
            /* When size == 0 the buffer must remain unchanged */
            /* No bytes to check, but the condition is trivially satisfied */
        }
    } else {
        /* When pBuf is NULL, bufsize is guaranteed to be 0 by the precondition */
        assert(bufsize == 0);
    }

    /* No other global state is modified by aws_secure_zero, so no further invariants to check */
}
