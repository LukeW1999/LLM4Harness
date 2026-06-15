#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* Prototype (if not already declared in common.h) */
void aws_secure_zero(void *pBuf, size_t bufsize);

void aws_secure_zero_harness(void) {
    /* 1. Nondeterministic inputs bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    bool p_is_null = nondet_bool();
    void *pBuf = NULL;

    if (!p_is_null) {
        /* Allocate a buffer of the given size */
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL);
        /* Initialize buffer with nondeterministic bytes */
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    /* Precondition implied by the implementation:
       if pBuf is NULL then bufsize must be 0 (otherwise the internal assert fails) */
    __CPROVER_assume(pBuf != NULL || bufsize == 0);

    /* 2. Save old state for immutability checks */
    struct store_byte_from_buffer old_mem;
    if (pBuf != NULL && bufsize > 0) {
        save_byte_from_array((uint8_t *)pBuf, bufsize, &old_mem);
    }

    /* 3. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 4. Postcondition checks */
    if (pBuf != NULL && bufsize > 0) {
        /* On success the buffer must be zeroed */
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            assert(bytes[i] == 0);
        }
    } else {
        /* Early return path: buffer (if any) must be unchanged */
        if (pBuf != NULL && bufsize == 0) {
            /* No bytes to check – nothing changes */
        } else {
            /* pBuf == NULL && bufsize == 0: nothing to check */
        }
    }

    /* 5. Unchanged inputs */
    /* pBuf and bufsize are passed by value; they cannot be modified by the function.
       No additional asserts are required for them. */

    /* 6. Clean up */
    if (pBuf != NULL) {
        free(pBuf);
    }
}
