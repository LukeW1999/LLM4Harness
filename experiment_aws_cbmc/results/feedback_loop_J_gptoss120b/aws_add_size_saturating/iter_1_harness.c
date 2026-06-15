#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include <aws/common/math.h>               /* for aws_secure_zero prototype */
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

/* The maximum size for the buffer – defined by the proof harness makefile */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256U
#endif

void aws_secure_zero_harness(void) {
    /* 1. Non‑deterministic size bounded by MAX_BUFFER_SIZE */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* 2. Allocate a buffer when size > 0, otherwise keep it NULL */
    uint8_t *pBuf = NULL;
    if (bufsize > 0) {
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL);
    }

    /* 3. Save the original contents of the buffer (if any) */
    struct store_byte_from_buffer old_bytes;
    if (pBuf != NULL && bufsize > 0) {
        save_byte_from_array(pBuf, bufsize, &old_bytes);
    }

    /* 4. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 5. Post‑conditions */

    /* 5a. When the pointer is NULL or the size is zero the function returns
        immediately – the memory must be unchanged. */
    if (pBuf == NULL || bufsize == 0) {
        if (pBuf != NULL && bufsize > 0) {
            assert_byte_from_buffer_matches(pBuf, &old_bytes);
        }
    } else {
        /* 5b. Otherwise every byte in the region must be zero. */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(pBuf[i] == 0);
        }
    }

    /* 6. The pointer itself must not be altered. */
    if (bufsize > 0) {
        assert(pBuf != NULL);
    } else {
        assert(pBuf == NULL);
    }

    /* 7. Clean up */
    free(pBuf);
}
