#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_secure_zero_harness() {
    /* 1. Declare nondeterministic size and bound it */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* 2. Allocate a buffer of the given size (may be NULL if size is 0) */
    uint8_t *pBuf = NULL;
    if (bufsize > 0) {
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL);
        /* Ensure the allocated memory is writable */
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(pBuf, bufsize));
    }

    /* 3. Save old contents of the buffer for later comparison */
    struct store_byte_from_buffer old_bytes;
    if (pBuf != NULL && bufsize > 0) {
        save_byte_from_array(pBuf, bufsize, &old_bytes);
    }

    /* 4. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 5. Postcondition checks */
    if (pBuf != NULL && bufsize > 0) {
        /* On success (non‑NULL pointer and non‑zero size) all bytes must be zero */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(pBuf[i] == 0);
        }
    } else {
        /* On failure (NULL pointer or zero size) the buffer must remain unchanged */
        if (pBuf != NULL && bufsize == 0) {
            /* No bytes to check, trivially unchanged */
        } else if (pBuf == NULL) {
            /* Nothing to check */
        } else {
            /* Should not reach here, but keep the invariant */
            assert_byte_from_buffer_matches(pBuf, &old_bytes);
        }
    }

    /* 6. Clean up */
    free(pBuf);
}
