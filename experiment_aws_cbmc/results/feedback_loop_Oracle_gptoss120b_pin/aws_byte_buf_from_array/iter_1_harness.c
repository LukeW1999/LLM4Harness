/* CBMC harness for aws_byte_buf_from_array */

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness(void) {
    /* nondeterministic length, bounded for tractability */
    size_t len = __CPROVER_nondet_size_t();
    __CPROVER_assume(len <= 256);

    /* allocate a writable array of the chosen length */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
        /* fill with nondeterministic data */
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = __CPROVER_nondet_uint8_t();
        }
    }

    /* keep a copy of the original contents for frame condition checking */
    uint8_t *bytes_copy = NULL;
    if (len > 0) {
        bytes_copy = malloc(len);
        __CPROVER_assume(bytes_copy != NULL);
        memcpy(bytes_copy, bytes, len);
    }

    /* call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_array(bytes, len);

    /* 1. Return value / error code correctness (validity predicate) */
    assert(aws_byte_buf_is_valid(&buf));

    /* 2. Output buffer length/capacity invariants */
    if (len == 0) {
        assert(buf.buffer == NULL);
    } else {
        assert(buf.buffer == bytes);
    }
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);

    /* 3. Memory not modified beyond the function's contract (frame condition) */
    if (len > 0) {
        for (size_t i = 0; i < len; ++i) {
            assert(bytes[i] == bytes_copy[i]);
        }
    }

    return 0;
}
