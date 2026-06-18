#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_secure_zero_harness(void) {
    /* nondeterministic buffer size, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* nondeterministic choice whether the pointer is NULL */
    bool ptr_is_null = nondet_bool();

    void *pBuf;
    uint8_t *allocated = NULL;

    if (!ptr_is_null) {
        /* allocate a buffer of the given size (may be zero) */
        allocated = (uint8_t *)malloc(bufsize);
        /* malloc may return NULL; treat that as a NULL pointer case */
        pBuf = allocated;
    } else {
        pBuf = NULL;
    }

    /* Save old memory contents for later comparison when needed */
    uint8_t *old_mem = NULL;
    if (pBuf != NULL && bufsize > 0) {
        old_mem = (uint8_t *)malloc(bufsize);
        __CPROVER_assume(old_mem != NULL);
        memcpy(old_mem, (uint8_t *)pBuf, bufsize);
    }

    /* Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* Postconditions */
    if (pBuf != NULL && bufsize > 0) {
        /* Memory must be zeroed */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(((uint8_t *)pBuf)[i] == 0);
        }
    } else {
        /* When pointer is NULL or size is zero, the function returns early.
         * If size is zero and pointer is non‑NULL, the buffer must remain unchanged. */
        if (pBuf != NULL && bufsize == 0) {
            /* No bytes to check; ensure the buffer content is unchanged (trivially true). */
            /* No additional asserts needed. */
        }
        /* If pBuf is NULL, there is no memory to inspect. */
    }

    /* Ensure that the allocator state (malloc/free) is not leaked beyond this harness.
     * Free any allocated memory. */
    if (allocated) {
        free(allocated);
    }
    if (old_mem) {
        free(old_mem);
    }
}
