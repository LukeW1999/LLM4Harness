#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include "aws/common/common.h"
#include "aws/common/math.h"
#include "proof_helpers/make_common_data_structures.h"

/* Prototype of the function under test */
void aws_secure_zero(void *pBuf, size_t bufsize);

void aws_secure_zero_harness(void) {
    /* 1. Declare nondeterministic inputs and bound them */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* nondeterministically decide whether the pointer is NULL */
    void *pBuf;
    if (nondet_bool()) {
        pBuf = NULL;
    } else {
        /* allocate a buffer of the chosen size (may be zero) */
        pBuf = malloc(bufsize);
        /* If allocation fails, treat it as NULL for the purpose of the harness */
        if (pBuf == NULL) {
            __CPROVER_assume(bufsize == 0);
        }
    }

    /* Save old state of inputs */
    void *old_pBuf = pBuf;
    size_t old_bufsize = bufsize;

    /* If we have a valid buffer, save its original contents for later comparison */
    struct store_byte_from_buffer storage;
    if (pBuf != NULL && bufsize > 0) {
        save_byte_from_array((uint8_t *)pBuf, bufsize, &storage);
    }

    /* 2. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 3. Post‑condition checks */

    /* The input arguments themselves must remain unchanged */
    assert(pBuf == old_pBuf);
    assert(bufsize == old_bufsize);

    if (pBuf == NULL || bufsize == 0) {
        /* Early return path: no memory should have been modified */
        if (pBuf != NULL && bufsize > 0) {
            assert_byte_from_buffer_matches((uint8_t *)pBuf, &storage);
        }
    } else {
        /* Successful zeroing: every byte must be 0 */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(((uint8_t *)pBuf)[i] == 0);
        }
    }

    /* 4. Validity invariants (the pointer must still be a valid writable region if non‑NULL) */
    if (pBuf != NULL && bufsize > 0) {
        __CPROVER_assert(AWS_MEM_IS_WRITABLE(pBuf, bufsize),
                         "memory region must be writable after aws_secure_zero");
    }

    /* Clean up */
    if (pBuf != NULL) {
        free(pBuf);
    }
}
