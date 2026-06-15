#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/proof_allocators.h"

void aws_byte_buf_init_copy_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator;

    /* Ensure src is bounded and valid */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Assume dest is uninitialized */
    dest.len = 0;
    dest.buffer = NULL;
    dest.capacity = 0;
    dest.allocator = NULL;

    /* Save src state before call */
    size_t old_len = src.len;
    size_t old_capacity = src.capacity;
    uint8_t *old_buffer = src.buffer;
    struct aws_allocator *old_allocator = src.allocator;

    /* Allocator may fail nondeterministically */
    allocator = can_fail_allocator();

    /* Call the function */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.allocator == allocator);
        assert(dest.len == old_len);
        assert(dest.capacity == old_capacity);
        assert(dest.buffer != NULL);
        if (old_len > 0) {
            assert_bytes_match(dest.buffer, old_buffer, old_len);
        }
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* On failure, dest should remain uninitialized */
        assert(dest.len == 0);
        assert(dest.buffer == NULL);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* src must remain unchanged */
    assert(src.len == old_len);
    assert(src.buffer == old_buffer);
    assert(src.capacity == old_capacity);
    assert(src.allocator == old_allocator);
    if (old_buffer != NULL && old_len > 0) {
        assert_bytes_match(src.buffer, old_buffer, old_len);
    }
}
