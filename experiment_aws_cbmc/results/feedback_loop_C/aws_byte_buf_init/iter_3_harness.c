#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_harness() {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t capacity = nondet_size_t();

    // Assume preconditions
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(capacity <= MAX_SIZE_T);

    int result = aws_byte_buf_init(&buf, allocator, capacity);

    if (result == AWS_OP_SUCCESS) {
        // Success path assertions
        assert(buf.buffer == (capacity == 0 ? NULL : (uint8_t *)any_ptr()));
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
    } else {
        // Failure path assertions
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == allocator); // Allocator should not change on failure
    }

    // Validity invariant
    assert(aws_byte_buf_is_valid(&buf));

    // Clean up
    aws_byte_buf_clean_up(&buf);
}
