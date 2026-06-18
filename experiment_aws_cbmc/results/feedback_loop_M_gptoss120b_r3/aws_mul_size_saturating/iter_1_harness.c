#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_secure_zero_harness(void) {
    /* 1. Non‑deterministic inputs, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    bool make_null = nondet_bool();
    void *pBuf = NULL;

    if (!make_null) {
        /* Allocate a buffer when we intend to zero it */
        if (bufsize > 0) {
            pBuf = malloc(bufsize);
            __CPROVER_assume(pBuf != NULL);
        } else {
            pBuf = NULL;
        }

        /* Save a copy of the original contents for later comparison */
        uint8_t *old = NULL;
        if (bufsize > 0) {
            old = malloc(bufsize);
            __CPROVER_assume(old != NULL);
            for (size_t i = 0; i < bufsize; ++i) {
                ((uint8_t *)pBuf)[i] = nondet_uint8_t();
                old[i] = ((uint8_t *)pBuf)[i];
            }
        }

        /* 2. Pre‑condition: the function asserts that a NULL buffer must have size 0 */
        __CPROVER_assume(!(pBuf == NULL && bufsize != 0));

        /* 3. Call the function under test */
        aws_secure_zero(pBuf, bufsize);

        /* 4. Post‑conditions */
        if (pBuf == NULL || bufsize == 0) {
            /* No memory to check; just ensure the pre‑condition held */
            assert(!(pBuf == NULL && bufsize != 0));
        } else {
            /* All bytes must be zero after the call */
            for (size_t i = 0; i < bufsize; ++i) {
                assert(((uint8_t *)pBuf)[i] == 0);
            }
        }

        /* 5. Clean up */
        if (pBuf) {
            free(pBuf);
        }
        if (old) {
            free(old);
        }
    } else {
        /* When we intentionally pass a NULL pointer */
        pBuf = NULL;

        /* Pre‑condition: NULL pointer requires size 0 */
        __CPROVER_assume(!(pBuf == NULL && bufsize != 0));

        aws_secure_zero(pBuf, bufsize);

        /* Post‑condition: function returns early, nothing to check */
        assert(!(pBuf == NULL && bufsize != 0));
    }
}
