#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include <proof_helpers/nondet.h>

/* Prototype of the function under test */
void aws_secure_zero(void *pBuf, size_t bufsize);

void aws_secure_zero_harness(void) {
    /* Bound for the buffer size to keep the state space manageable */
    const size_t MAX_BUF_SIZE = 64;

    /* Non‑deterministic inputs */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUF_SIZE);

    void *pBuf;
    /* Decide non‑deterministically whether the pointer is NULL */
    if (nondet_bool()) {
        pBuf = NULL;
    } else {
        /* Allocate a buffer of the given size (may be zero) */
        pBuf = malloc(bufsize);
        /* If allocation fails, treat it as a zero‑length buffer */
        __CPROVER_assume(pBuf != NULL || bufsize == 0);
        if (pBuf) {
            /* Fill the buffer with nondeterministic bytes */
            uint8_t *bytes = (uint8_t *)pBuf;
            for (size_t i = 0; i < bufsize; ++i) {
                bytes[i] = nondet_uint8_t();
            }
        }
    }

    /* The function asserts that if pBuf is NULL then bufsize must be 0 */
    if (pBuf == NULL) {
        __CPROVER_assume(bufsize == 0);
    }

    /* Save the original pointer value for later comparison */
    void *old_pBuf = pBuf;

    /* Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* Post‑conditions */
    if (pBuf != NULL && bufsize > 0) {
        /* The buffer must be zeroed */
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            assert(bytes[i] == 0);
        }
    }

    /* The pointer itself must not be modified */
    assert(pBuf == old_pBuf);
}
