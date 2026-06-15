#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <aws/common/common.h>
#include <aws/common/math.h>

/* Maximum buffer size for the harness – adjust as needed for the verification environment */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void aws_secure_zero_harness(void) {
    /* 1. Declare nondeterministic size and ensure it is bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* 2. Allocate a larger buffer to observe unchanged regions */
    uint8_t *bigbuf = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(bigbuf != NULL);
    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        bigbuf[i] = nondet_uint8_t();
    }

    /* 3. Choose a nondeterministic offset such that the region [offset, offset+bufsize) fits */
    size_t offset = nondet_size_t();
    __CPROVER_assume(offset <= MAX_BUFFER_SIZE);
    __CPROVER_assume(offset + bufsize <= MAX_BUFFER_SIZE);

    uint8_t *pBuf = bigbuf + offset;

    /* 4. Save a copy of the original buffer for later comparison */
    uint8_t *old = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(old != NULL);
    memcpy(old, bigbuf, MAX_BUFFER_SIZE);

    /* 5. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 6. Post‑condition checks */
    if (pBuf != NULL && bufsize > 0) {
        /* The specified region must be zeroed */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(pBuf[i] == 0);
        }
        /* Bytes before the region must be unchanged */
        for (size_t i = 0; i < offset; ++i) {
            assert(bigbuf[i] == old[i]);
        }
        /* Bytes after the region must be unchanged */
        for (size_t i = offset + bufsize; i < MAX_BUFFER_SIZE; ++i) {
            assert(bigbuf[i] == old[i]);
        }
    } else {
        /* No memory should have been modified */
        for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
            assert(bigbuf[i] == old[i]);
        }
    }

    /* 7. Clean up */
    free(old);
    free(bigbuf);
}
