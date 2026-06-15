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
    /* 1. Allocate a buffer that may be used as the source array */
    uint8_t *bytes = (uint8_t *)malloc(MAX_ARRAY_SIZE);
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_ARRAY_SIZE);

    /* 2. Nondeterministically make the source pointer NULL to explore failure case */
    if (nondet_bool()) {
        free(bytes);
        bytes = NULL;
    }

    /* 3. Prepare the destination byte buffer */
    struct aws_byte_buf dest = {0};

    /* 4. Call the function under test */
    int result = aws_byte_buf_from_array(&dest, bytes, len);

    /* 5. Assert post‑conditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* The buffer should reference the provided array (or be NULL if length is zero) */
        assert(dest.allocator == NULL);
        assert(dest.capacity == len);
        assert(dest.len == len);
        if (len == 0) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer == bytes);
        }
    } else {
        /* On failure the destination should be left empty */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 6. The source array must remain unchanged (its contents are not modified) */
    /* No direct assertion needed; the function does not write to `bytes`. */

    /* 7. The destination byte buffer must always satisfy its invariant */
    assert(aws_byte_buf_is_valid(&dest));
}
