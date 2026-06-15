#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Proof helper declarations (provided by the verification environment) */
bool nondet_bool(void);
size_t nondet_size_t(void);
bool AWS_MEM_IS_WRITABLE(void *ptr, size_t size);

/* Function under test */
extern void aws_secure_zero(void *pBuf, size_t bufsize);

/* Bounding constant – defined by the Makefile in the verification setup */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void aws_secure_zero_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* 2. Choose whether the pointer is NULL or points to a writable buffer */
    void *pBuf;
    if (nondet_bool()) {
        pBuf = NULL;
    } else {
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL || bufsize == 0);
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(pBuf, bufsize));
    }

    /* 3. Save old contents of the buffer (if any) */
    uint8_t *old = NULL;
    if (pBuf != NULL && bufsize > 0) {
        old = malloc(bufsize);
        __CPROVER_assume(old != NULL);
        memcpy(old, pBuf, bufsize);
    }

    /* 4. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 5. Post‑condition checks */
    if (pBuf != NULL && bufsize > 0) {
        /* Success path – the buffer must be zeroed */
        for (size_t i = 0; i < bufsize; ++i) {
            __CPROVER_assert(((uint8_t *)pBuf)[i] == 0,
                             "aws_secure_zero: each byte of the buffer is zero after a non‑zero length call");
        }
    } else {
        /* Early‑return path – the buffer (if any) must be unchanged */
        if (old != NULL) {
            for (size_t i = 0; i < bufsize; ++i) {
                __CPROVER_assert(((uint8_t *)pBuf)[i] == old[i],
                                 "aws_secure_zero: buffer unchanged on early return");
            }
        }
    }

    /* 6. Clean up */
    free(old);
    free(pBuf);
}
