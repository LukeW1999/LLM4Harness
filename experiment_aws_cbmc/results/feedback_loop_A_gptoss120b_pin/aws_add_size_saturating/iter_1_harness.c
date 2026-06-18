#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_secure_zero_harness(void) {
    /* nondeterministic size bounded by MAX_BUFFER_SIZE */
    size_t size = nondet_size_t();
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* nondeterministic pointer (may be NULL) */
    uint8_t *buf = NULL;
    if (size > 0) {
        /* allocate buffer when size > 0 */
        buf = malloc(size);
        __CPROVER_assume(buf != NULL);
        /* fill with nondet data */
        for (size_t i = 0; i < size; ++i) {
            buf[i] = nondet_uint8_t();
        }
    }

    /* Save old state */
    uint8_t *old_buf = buf;
    size_t old_size = size;
    uint8_t *old_contents = NULL;
    if (buf != NULL && size > 0) {
        old_contents = malloc(size);
        __CPROVER_assume(old_contents != NULL);
        for (size_t i = 0; i < size; ++i) {
            old_contents[i] = buf[i];
        }
    }

    /* Call function under test */
    aws_secure_zero(buf, size);

    /* Postconditions */
    /* Pointer and size must remain unchanged */
    assert(buf == old_buf);
    assert(size == old_size);

    if (buf != NULL && size > 0) {
        /* Success path: all bytes must be zero */
        for (size_t i = 0; i < size; ++i) {
            assert(buf[i] == 0);
        }
    } else {
        /* Failure path: buffer contents must be unchanged */
        if (buf != NULL && size > 0) {
            for (size_t i = 0; i < size; ++i) {
                assert(buf[i] == old_contents[i]);
            }
        }
        /* If buf is NULL or size == 0, there is nothing to check */
    }

    /* Clean up */
    if (buf != NULL && size > 0) {
        free(buf);
        free(old_contents);
    }
}
