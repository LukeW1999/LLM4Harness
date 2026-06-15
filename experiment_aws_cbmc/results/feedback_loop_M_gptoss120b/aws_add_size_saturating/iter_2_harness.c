#include <aws/common/common.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

/* Function under test */
void aws_secure_zero(void *pBuf, size_t bufsize);

void aws_secure_zero_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Nondeterministic inputs, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    bool ptr_is_null = nondet_bool();

    void *pBuf = NULL;
    void *old_pBuf = NULL;

    if (!ptr_is_null) {
        if (bufsize > 0) {
            pBuf = aws_mem_acquire(allocator, bufsize);
            __CPROVER_assume(pBuf != NULL);
            uint8_t *bytes = (uint8_t *)pBuf;
            for (size_t i = 0; i < bufsize; ++i) {
                bytes[i] = nondet_uint8_t();
            }
        } else {
            /* bufsize == 0, allocate a non‑NULL pointer (allowed) */
            pBuf = aws_mem_acquire(allocator, 1);
            __CPROVER_assume(pBuf != NULL);
        }
    } else {
        pBuf = NULL;
    }

    old_pBuf = pBuf; /* save pointer value */

    /* 2. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 3. Post‑condition checks */
    assert(!(pBuf == NULL && bufsize != 0));
    assert(pBuf == old_pBuf);

    if (pBuf != NULL && bufsize > 0) {
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            assert(bytes[i] == 0);
        }
    }

    /* 4. Clean up */
    if (pBuf != NULL) {
        aws_mem_release(allocator, pBuf);
    }
}
