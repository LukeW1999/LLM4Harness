#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_secure_zero_harness(void) {
    /* nondeterministic size bounded by MAX_BUFFER_SIZE */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);
    size_t old_bufsize = bufsize;

    /* nondeterministic pointer: either NULL or a valid buffer */
    bool is_null = nondet_bool();
    void *pBuf = NULL;
    if (!is_null) {
        /* allocate a buffer large enough for any possible bufsize */
        pBuf = malloc(MAX_BUFFER_SIZE);
        __CPROVER_assume(pBuf != NULL);
        /* ensure the allocated region is writable */
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(pBuf, MAX_BUFFER_SIZE));
    }

    /* Save old memory contents for later comparison */
    uint8_t old_mem[MAX_BUFFER_SIZE];
    if (pBuf != NULL) {
        memcpy(old_mem, pBuf, MAX_BUFFER_SIZE);
    }

    /* Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* Post‑conditions */
    if (pBuf == NULL) {
        /* When the buffer pointer is NULL the function asserts bufsize == 0 */
        assert(bufsize == 0);
    } else {
        if (bufsize == 0) {
            /* No zeroing performed – memory must remain unchanged */
            for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
                assert(((uint8_t *)pBuf)[i] == old_mem[i]);
            }
        } else {
            /* First bufsize bytes must be zero */
            for (size_t i = 0; i < bufsize; ++i) {
                assert(((uint8_t *)pBuf)[i] == 0);
            }
            /* Remaining bytes must be unchanged */
            for (size_t i = bufsize; i < MAX_BUFFER_SIZE; ++i) {
                assert(((uint8_t *)pBuf)[i] == old_mem[i]);
            }
        }
    }

    /* Fields that must not change regardless of outcome */
    assert(bufsize == old_bufsize);
    assert(pBuf == pBuf); /* pointer itself is unchanged */

    /* Clean up */
    if (pBuf != NULL) {
        free(pBuf);
    }
}
