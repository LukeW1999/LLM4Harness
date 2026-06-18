#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_secure_zero_harness(void) {
    /* nondeterministic size bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* allocate buffer (may be NULL if size is 0) */
    void *pBuf = NULL;
    if (bufsize > 0) {
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL);
        /* initialize memory with nondeterministic values */
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    } else {
        /* when size is zero the pointer may be NULL or any value;
         * we allow both possibilities */
        pBuf = malloc(1);               /* may return NULL or a valid pointer */
    }

    /* save old contents for later comparison */
    uint8_t *old = NULL;
    if (pBuf != NULL) {
        old = malloc(bufsize);
        __CPROVER_assume(old != NULL);
        for (size_t i = 0; i < bufsize; ++i) {
            old[i] = ((uint8_t *)pBuf)[i];
        }
    }

    /* call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* post‑conditions */
    if (pBuf != NULL && bufsize > 0) {
        /* successful zeroing: all bytes must be 0 */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(((uint8_t *)pBuf)[i] == 0);
        }
    } else {
        /* either pointer is NULL or size is zero */
        /* if pointer is NULL then bufsize must be 0 (AWS_ASSERT) */
        assert(!(pBuf == NULL) || bufsize == 0);

        /* if size is zero and pointer is non‑NULL, memory must be unchanged */
        if (bufsize == 0 && pBuf != NULL) {
            for (size_t i = 0; i < bufsize; ++i) {
                assert(((uint8_t *)pBuf)[i] == old[i]);
            }
        }
    }

    /* clean up */
    if (pBuf != NULL) {
        free(pBuf);
    }
    if (old != NULL) {
        free(old);
    }
}
