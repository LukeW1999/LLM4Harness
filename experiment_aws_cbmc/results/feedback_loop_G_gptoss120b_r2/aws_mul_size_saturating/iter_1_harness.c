#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* The maximum size for the buffer; defined by the proof environment */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void aws_secure_zero_harness(void) {
    /* Allocate a maximal buffer that can be used for the test */
    uint8_t *allocated = (uint8_t *)malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(allocated != NULL);

    /* Non‑deterministically decide whether the pointer passed to the function is NULL */
    bool ptr_is_null = nondet_bool();
    void *pBuf = ptr_is_null ? NULL : (void *)allocated;

    /* Non‑deterministically choose a size, bounded by the allocated buffer */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* Save a copy of the original contents of the allocated buffer */
    uint8_t *old_contents = (uint8_t *)malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(old_contents != NULL);
    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        old_contents[i] = allocated[i];
    }

    /* Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* Post‑conditions */
    if (pBuf != NULL && bufsize != 0) {
        /* The first `bufsize` bytes must be zero */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(((uint8_t *)pBuf)[i] == 0);
        }
        /* Bytes beyond `bufsize` must remain unchanged */
        for (size_t i = bufsize; i < MAX_BUFFER_SIZE; ++i) {
            assert(((uint8_t *)pBuf)[i] == old_contents[i]);
        }
    } else {
        /* No modification must occur */
        if (allocated != NULL) {
            for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
                assert(allocated[i] == old_contents[i]);
            }
        }
    }

    /* Clean up */
    free(old_contents);
    free(allocated);
}
