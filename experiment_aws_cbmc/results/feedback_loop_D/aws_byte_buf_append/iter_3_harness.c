#include <aws/common/byte_buf.h>
#include <aws/common/byte_order.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_byte_buf_append_harness() {
    struct aws_byte_buf to;
    struct aws_byte_cursor from;
    struct aws_byte_buf old_to;
    struct aws_byte_cursor old_from;

    // Initialize to with arbitrary values
    make_byte_buf(&to, nondet_size_t(), nondet_size_t());
    struct aws_allocator *allocator = aws_default_allocator();
    to.allocator = allocator;

    // Initialize from with arbitrary values
    make_byte_cursor(&from, nondet_size_t());

    // Ensure preconditions
    assume(aws_byte_buf_is_valid(&to));
    assume(aws_byte_cursor_is_valid(&from));
    assume(to.capacity >= to.len + from.len); // Ensure there's enough capacity

    // Copy original values for postcondition checks
    old_to = to;
    old_from = from;

    int result = aws_byte_buf_append(&to, &from);

    // Postconditions
    if (result == AWS_OP_SUCCESS) {
        assert(to.len == old_to.len + from.len);
        if (from.len > 0) {
            assert_byte_from_buffer_matches(to.buffer + old_to.len, from.ptr, from.len);
        }
    } else {
        assert(to.len == old_to.len);
        assert(to.buffer == old_to.buffer);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
    }

    // Frame conditions
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    // Validity invariants
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
