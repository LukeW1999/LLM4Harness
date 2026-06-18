#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_secure_zero_harness(void) {
    /* nondeterministic inputs */
    bool is_null = nondet_bool();
    size_t bufsize = nondet_size_t();

    /* bound the size to keep the state space reasonable */
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* allocate buffer if needed */
    void *pBuf;
    if (!is_null) {
        /* when buffer is non‑NULL, size must be > 0 (otherwise the function would take the early‑return path) */
        __CPROVER_assume(bufsize > 0);
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL);
    } else {
        /* when buffer is NULL the function requires bufsize == 0 */
        __CPROVER_assume(bufsize == 0);
        pBuf = NULL;
    }

    /* Save old state for later comparison */
    void *old_pBuf = pBuf;
    size_t old_bufsize = bufsize;

    /* Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* Post‑conditions */

    /* 1. If the buffer was non‑NULL and size > 0, all bytes must be zero */
    if (old_pBuf != NULL && old_bufsize > 0) {
        unsigned char *bytes = (unsigned char *)old_pBuf;
        for (size_t i = 0; i < old_bufsize; ++i) {
            assert(bytes[i] == 0);
        }
    }

    /* 2. The pointer itself must not change */
    assert(pBuf == old_pBuf);

    /* 3. The size argument is unchanged (it is a copy, but we assert the local variable stayed the same) */
    assert(bufsize == old_bufsize);

    /* 4. No memory safety violations – the buffer (if any) remains writable for its size */
    if (pBuf != NULL && bufsize > 0) {
        /* CBMC built‑in predicate to ensure the region is still writable */
        __CPROVER_assert(__CPROVER_w_ok(pBuf, bufsize), "buffer must be writable after zeroing");
    }
}
