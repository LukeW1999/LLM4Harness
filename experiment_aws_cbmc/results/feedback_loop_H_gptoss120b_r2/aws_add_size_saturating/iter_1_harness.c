#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_secure_zero_harness(void) {
    /* nondeterministic size, bounded */
    size_t size = nondet_size_t();
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* nondeterministic pointer: either NULL or a valid allocated buffer */
    uint8_t *buf = NULL;
    bool have_buf = nondet_bool();
    if (have_buf) {
        /* allocate buffer of the given size */
        buf = (uint8_t *)malloc(size);
        __CPROVER_assume(buf != NULL);
        /* make the buffer readable/writable for CBMC */
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(buf, size));
        /* initialize buffer with nondeterministic data */
        for (size_t i = 0; i < size; ++i) {
            buf[i] = nondet_uint8_t();
        }
    }

    /* Save old state of the buffer (if any) */
    uint8_t *old_buf = NULL;
    if (buf != NULL && size > 0) {
        old_buf = (uint8_t *)malloc(size);
        __CPROVER_assume(old_buf != NULL);
        for (size_t i = 0; i < size; ++i) {
            old_buf[i] = buf[i];
        }
    }

    /* Call the function under test */
    aws_secure_zero(buf, size);

    /* Postconditions */
    if (buf == NULL || size == 0) {
        /* No modification should have occurred */
        if (old_buf != NULL) {
            for (size_t i = 0; i < size; ++i) {
                assert(buf[i] == old_buf[i]);
            }
        }
    } else {
        /* Buffer should be zeroed */
        for (size_t i = 0; i < size; ++i) {
            assert(buf[i] == 0);
        }
    }

    /* Pointer itself must remain unchanged */
    if (have_buf) {
        assert(buf == buf);
    } else {
        assert(buf == NULL);
    }

    /* Clean up */
    if (buf != NULL) {
        free(buf);
    }
    if (old_buf != NULL) {
        free(old_buf);
    }
}
