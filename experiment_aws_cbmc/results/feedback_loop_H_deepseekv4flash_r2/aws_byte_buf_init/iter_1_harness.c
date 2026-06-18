#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_harness() {
    /* 1. Declare structs and allocate as needed */
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 2. Save old state (not needed, but for completeness) */
    struct aws_byte_buf old;
    // No need to copy because function overwrites all fields.

    /* 3. Call function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 4. Assert postconditions for both success and failure */
    if (result == AWS_OP_SUCCESS) {
        /* success: buffer allocated (or NULL if capacity == 0), len == 0, capacity == capacity, allocator == allocator */
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
    } else {
        /* failure: structure zeroed by AWS_ZERO_STRUCT */
        assert(buf.len == 0);
        assert(buf.buffer == NULL);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result: none, all are overwritten */

    /* 6. Assert validity invariants */
    assert(aws_byte_buf_is_valid(&buf));
}
