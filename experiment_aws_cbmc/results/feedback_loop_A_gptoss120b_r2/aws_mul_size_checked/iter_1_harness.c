#include <aws/common/common.h>
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

    /* nondeterministic choice whether the buffer pointer is NULL */
    bool ptr_is_null = nondet_bool();

    void *pBuf = NULL;
    void *old_ptr = NULL;
    uint8_t *old_contents = NULL;

    if (!ptr_is_null) {
        /* allocate a buffer of the chosen size */
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL);
        old_ptr = pBuf;

        /* fill buffer with nondeterministic data */
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* save a copy of the original contents for potential later checks */
        old_contents = malloc(bufsize);
        __CPROVER_assume(old_contents != NULL);
        for (size_t i = 0; i < bufsize; ++i) {
            old_contents[i] = bytes[i];
        }
    }

    /* Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* Post‑condition checks */
    if (pBuf != NULL && bufsize != 0) {
        /* The pointer itself must remain unchanged */
        assert(pBuf == old_ptr);

        /* All bytes in the buffer must be zeroed */
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            assert(bytes[i] == 0);
        }
    } else {
        /* If either argument triggered the early‑return path, the function asserts that bufsize == 0 */
        assert(pBuf == NULL || bufsize == 0);
        assert(bufsize == 0);
    }

    /* No additional validity invariants are required for this function */
}
