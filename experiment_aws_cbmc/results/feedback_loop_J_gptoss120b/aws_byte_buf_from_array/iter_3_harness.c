#include <aws/common/byte_buf.h>
#include <aws/common/assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_ARRAY_SIZE 256

void aws_byte_buf_from_array_harness(void) {
    /* Allocate a source array */
    uint8_t *bytes = (uint8_t *)malloc(MAX_ARRAY_SIZE);
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_ARRAY_SIZE);

    /* Nondeterministically make the source pointer NULL */
    if (nondet_bool()) {
        free(bytes);
        bytes = NULL;
    }

    /* Ensure the failure path is reachable when bytes is NULL */
    if (bytes == NULL) {
        __CPROVER_assume(len > 0);
    }

    /* Destination buffer */
    struct aws_byte_buf dest = {0};

    /* Call the function under test */
    int result = aws_byte_buf_from_array(&dest, bytes, len);

    /* Post‑conditions */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.allocator == NULL);
        assert(dest.capacity == len);
        assert(dest.len == len);
        if (len == 0) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer == bytes);
        }
    } else {
        /* On failure the destination should remain empty */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* Invariant must always hold */
    assert(aws_byte_buf_is_valid(&dest));
}
