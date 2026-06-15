#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

/* Prototype of the function under test */
void aws_secure_zero(void *pBuf, size_t bufsize);

/* Helper nondeterministic generators (provided by the proof environment) */
bool nondet_bool(void);
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

/* Bounding macro (defined in the proof makefile) */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void aws_secure_zero_harness(void) {
    /* 1. Declare inputs and nondeterministically initialize them */
    size_t bufsize = nondet_size_t();

    void *pBuf;
    if (nondet_bool()) {
        /* pBuf may be NULL */
        pBuf = NULL;
    } else {
        /* pBuf points to a writable buffer */
        __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL);

        /* Fill the buffer with nondeterministic data */
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    /* 2. Impose the function's precondition:
       if pBuf is NULL then bufsize must be 0 (otherwise the internal assert would fire) */
    __CPROVER_assume(pBuf != NULL || bufsize == 0);

    /* 3. Save old state that must remain unchanged */
    void *old_pBuf = pBuf;

    /* 4. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 5. Assert post‑conditions */

    /* 5a. The pointer itself must not change */
    assert(pBuf == old_pBuf);

    /* 5b. If a non‑null buffer of non‑zero size was provided, every byte must be zeroed */
    if (old_pBuf != NULL && bufsize > 0) {
        uint8_t *bytes = (uint8_t *)old_pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            assert(bytes[i] == 0);
        }
    }

    /* 5c. If bufsize is zero (or pBuf is NULL), there is nothing else to assert;
       the function returns early without touching memory. */
}
