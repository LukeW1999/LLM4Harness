#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_harness() {
    /* 1. Declare inputs */
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t capacity;

    /* Bound capacity to keep state space manageable */
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* Preconditions: buf and allocator must be non-NULL */
    /* buf is stack-allocated so non-NULL; allocator from aws_default_allocator() is non-NULL */

    /* 2. Call function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 3. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: len == 0, capacity == capacity, allocator set, buffer valid */
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        /* If capacity == 0, buffer must be NULL; otherwise buffer must be non-NULL */
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
        /* Validity invariant */
        assert(aws_byte_buf_is_valid(&buf));
    } else {
        /* On failure: AWS_ZERO_STRUCT was applied, so all fields are zero/NULL */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
        assert(buf.buffer == NULL);
        /* Validity invariant: capacity == 0 and buffer == NULL satisfies the predicate */
        assert(aws_byte_buf_is_valid(&buf));
    }
}
