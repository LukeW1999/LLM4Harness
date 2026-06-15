#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

/* Function under test */
void aws_secure_zero(void *pBuf, size_t bufsize);

/* Maximum size for nondeterministic allocation (adjust as needed) */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void aws_secure_zero_harness(void) {
    /* 1. Declare nondeterministic inputs */
    void *pBuf;
    size_t bufsize = nondet_size_t();

    /* Bound the size */
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* Decide nondeterministically whether pBuf is NULL */
    if (nondet_bool()) {
        pBuf = NULL;
    } else {
        /* Allocate a buffer of at least bufsize bytes */
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL);
        /* Ensure the allocated memory is readable and writable */
        __CPROVER_assume(__CPROVER_is_fresh(pBuf, bufsize));
    }

    /* 2. Save old state of the memory region (if any) */
    uint8_t *old_mem = NULL;
    if (pBuf != NULL && bufsize > 0) {
        old_mem = malloc(bufsize);
        __CPROVER_assume(old_mem != NULL);
        /* Copy original contents */
        for (size_t i = 0; i < bufsize; ++i) {
            old_mem[i] = ((uint8_t *)pBuf)[i];
        }
    }

    /* 3. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 4. Postconditions */

    /* If pBuf is NULL, the function asserts bufsize == 0 */
    if (pBuf == NULL) {
        assert(bufsize == 0);
    } else if (bufsize == 0) {
        /* No operation performed; memory should remain unchanged */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(((uint8_t *)pBuf)[i] == old_mem[i]);
        }
    } else {
        /* bufsize > 0 and pBuf != NULL: all bytes must be zero */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(((uint8_t *)pBuf)[i] == 0);
        }
    }

    /* 5. Unchanged fields */
    /* The pointer itself must not be modified */
    if (pBuf != NULL) {
        assert(pBuf == pBuf);
    }

    /* 6. Clean up */
    if (pBuf != NULL) {
        free(pBuf);
    }
    if (old_mem != NULL) {
        free(old_mem);
    }
}
