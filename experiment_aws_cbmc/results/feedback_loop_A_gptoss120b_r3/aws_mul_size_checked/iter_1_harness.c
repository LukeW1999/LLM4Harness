#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_secure_zero_harness(void) {
    /* nondeterministic size, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* nondeterministic pointer choice */
    void *pBuf;
    bool is_null = nondet_bool();

    if (is_null) {
        pBuf = NULL;
        /* Precondition: when pBuf is NULL, bufsize must be 0 to satisfy the internal assert */
        __CPROVER_assume(bufsize == 0);
    } else {
        /* allocate a buffer of the given size */
        if (bufsize == 0) {
            pBuf = NULL; /* malloc(0) may return NULL or a unique pointer; we treat it as NULL */
        } else {
            pBuf = malloc(bufsize);
            __CPROVER_assume(pBuf != NULL);
            /* ensure the allocated memory is writable */
            __CPROVER_assume(AWS_MEM_IS_WRITABLE(pBuf, bufsize));
        }
    }

    /* Save a copy of the original memory for later comparison if needed */
    unsigned char *orig = NULL;
    if (pBuf != NULL && bufsize > 0) {
        orig = malloc(bufsize);
        __CPROVER_assume(orig != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(pBuf, bufsize));
        for (size_t i = 0; i < bufsize; ++i) {
            ((unsigned char *)orig)[i] = ((unsigned char *)pBuf)[i];
        }
    }

    /* Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* Postconditions */
    if (pBuf != NULL && bufsize > 0) {
        /* All bytes must be zero after the call */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(((unsigned char *)pBuf)[i] == 0);
        }
    } else {
        /* When pBuf is NULL or bufsize is 0, nothing to assert about memory contents */
        /* The function should have returned early without touching memory */
    }

    /* Clean up */
    if (orig) {
        free(orig);
    }
    if (pBuf) {
        free(pBuf);
    }
}
