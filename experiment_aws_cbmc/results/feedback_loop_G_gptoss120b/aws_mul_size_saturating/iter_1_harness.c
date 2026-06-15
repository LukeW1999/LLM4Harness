#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <aws/common/common.h>

/* Maximum size for the buffer we will reason about */
#define MAX_BUF_SIZE 64

void aws_secure_zero_harness(void) {
    /* 1. Declare nondeterministic inputs */
    size_t bufsize = nondet_size_t();
    bool use_null = nondet_bool();

    /* Bound the size */
    __CPROVER_assume(bufsize <= MAX_BUF_SIZE);

    /* Allocate a concrete buffer */
    uint8_t buf[MAX_BUF_SIZE];
    /* Initialize buffer with nondeterministic values */
    for (size_t i = 0; i < MAX_BUF_SIZE; ++i) {
        buf[i] = nondet_uint8_t();
    }

    /* Save old state of the whole buffer */
    uint8_t old_buf[MAX_BUF_SIZE];
    memcpy(old_buf, buf, MAX_BUF_SIZE);

    /* Choose pointer argument */
    void *pBuf = use_null ? NULL : (void *)buf;

    /* The function asserts that if pBuf is NULL then bufsize must be 0 */
    __CPROVER_assume(pBuf != NULL || bufsize == 0);

    /* 2. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 3. Post‑condition checks */

    if (pBuf != NULL && bufsize > 0) {
        /* The first bufsize bytes must be zero */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(buf[i] == 0);
        }
        /* Bytes beyond bufsize must be unchanged */
        for (size_t i = bufsize; i < MAX_BUF_SIZE; ++i) {
            assert(buf[i] == old_buf[i]);
        }
    } else {
        /* No memory was touched; entire buffer must be unchanged */
        for (size_t i = 0; i < MAX_BUF_SIZE; ++i) {
            assert(buf[i] == old_buf[i]);
        }
    }

    /* 4. Invariant: the function never writes outside the provided region */
    /* (already covered by the checks above) */
}
