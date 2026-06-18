#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*  Harness for aws_secure_zero() */
void aws_secure_zero_harness(void) {
    /* 1. Non‑deterministic size, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);   /* bound the state space */

    /* 2. Allocate a buffer according to the size */
    uint8_t *pBuf = NULL;
    if (bufsize > 0) {
        pBuf = malloc(bufsize);
        /* malloc may fail – that is a legitimate case */
    }

    /* 3. Remember the original state */
    uint8_t *old_pBuf = pBuf;
    size_t   old_bufsize = bufsize;

    /* 4. Initialise the buffer with nondet data (if it exists) */
    if (pBuf != NULL) {
        for (size_t i = 0; i < bufsize; ++i) {
            pBuf[i] = nondet_uint8_t();
        }
    }

    /* 5. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 6. Post‑condition: pointer and size must be unchanged */
    assert(pBuf == old_pBuf);
    assert(bufsize == old_bufsize);

    /* 7. Behaviour when buffer is NULL or size is zero */
    if (pBuf == NULL || bufsize == 0) {
        /* The implementation asserts that bufsize == 0 in this path */
        assert(bufsize == 0);
    } else {
        /* 8. When a valid buffer and non‑zero size are provided,
           every byte in the range [0, bufsize) must be zero. */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(pBuf[i] == 0);
        }
    }

    /* 9. Clean up */
    free(pBuf);
}
