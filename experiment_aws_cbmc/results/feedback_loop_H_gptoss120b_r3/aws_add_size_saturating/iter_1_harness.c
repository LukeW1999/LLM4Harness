#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* The harness for aws_secure_zero */
void aws_secure_zero_harness(void) {
    /* 1. Non‑deterministic size, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* 2. Non‑deterministic pointer (NULL or allocated) */
    uint8_t *pBuf = NULL;
    bool allocate = nondet_bool();

    if (allocate) {
        /* Allocate a buffer of the chosen size */
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL);
        /* Ensure the buffer is readable/writable for the whole range */
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(pBuf, bufsize));
    } else {
        /* Keep NULL; in this case bufsize must be zero to satisfy the function's internal assert */
        __CPROVER_assume(bufsize == 0);
        pBuf = NULL;
    }

    /* 3. Save old state */
    uint8_t *old_pBuf = pBuf;
    size_t old_bufsize = bufsize;

    /* 4. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 5. Post‑condition checks */
    if (old_pBuf != NULL) {
        /* The pointer must remain unchanged */
        assert(pBuf == old_pBuf);
        /* The size must remain unchanged */
        assert(bufsize == old_bufsize);

        /* The memory region must be zeroed */
        uint8_t *zero = malloc(bufsize);
        __CPROVER_assume(zero != NULL);
        memset(zero, 0, bufsize);
        assert_bytes_match(pBuf, zero, bufsize);
        free(zero);
    } else {
        /* When the pointer is NULL the function asserts that bufsize is zero */
        assert(bufsize == 0);
    }

    /* 6. Clean up */
    if (old_pBuf != NULL) {
        free(old_pBuf);
    }
}
