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
    size_t capacity = nondet_size_t();

    /* Bound capacity to keep state space manageable */
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* Preconditions: buf and allocator must be non-NULL */
    /* buf is stack-allocated so it's non-NULL; allocator is from aws_default_allocator() */
    __CPROVER_assume(allocator != NULL);

    /* 2. Call function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 3. Assert postconditions */

    /* Return value must be either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1) */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - buf.len == 0
         * - buf.capacity == capacity
         * - buf.allocator == allocator
         * - if capacity == 0, buf.buffer == NULL
         * - if capacity != 0, buf.buffer != NULL
         */
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
        /* Validity invariant */
        assert(aws_byte_buf_is_valid(&buf));
    } else {
        /* On failure (AWS_OP_ERR):
         * - The implementation does AWS_ZERO_STRUCT(*buf), so all fields are zeroed
         * - buf.len == 0
         * - buf.capacity == 0
         * - buf.buffer == NULL
         * - buf.allocator == NULL
         */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
        /* Validity invariant still holds (zeroed struct is valid: len==0 <= capacity==0, capacity==0 so buffer can be NULL) */
        assert(aws_byte_buf_is_valid(&buf));
    }

    /* Invariant: len <= capacity always */
    assert(buf.len <= buf.capacity);

    /* Invariant: if capacity != 0 then buffer != NULL */
    assert(buf.capacity == 0 || buf.buffer != NULL);
}
