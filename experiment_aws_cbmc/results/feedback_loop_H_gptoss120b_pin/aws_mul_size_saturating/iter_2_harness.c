#include <aws/common/math.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 64

void aws_secure_zero_harness() {
    /* nondeterministic buffer size, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* nondeterministically decide if the pointer is NULL */
    bool ptr_is_null = nondet_bool();

    void *pBuf = NULL;

    if (!ptr_is_null) {
        /* allocate a buffer of the given size */
        pBuf = malloc(bufsize);
        /* if allocation fails, treat as zero-size buffer */
        __CPROVER_assume(pBuf != NULL || bufsize == 0);

        if (bufsize > 0) {
            /* initialize buffer with nondeterministic data */
            unsigned char *bytes = (unsigned char *)pBuf;
            for (size_t i = 0; i < bufsize; ++i) {
                bytes[i] = nondet_uint8_t();
            }
        }
    }

    /* Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* Postconditions */
    if (pBuf == NULL) {
        /* The function asserts that bufsize == 0 when pBuf is NULL */
        assert(bufsize == 0);
    } else if (bufsize > 0) {
        /* All bytes must be zero */
        unsigned char *bytes = (unsigned char *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            assert(bytes[i] == 0);
        }
    }

    /* Clean up */
    free(pBuf);
}
