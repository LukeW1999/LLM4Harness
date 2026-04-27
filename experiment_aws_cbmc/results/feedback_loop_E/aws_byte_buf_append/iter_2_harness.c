#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_append_harness() {
    /* data structures */
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    /* bounds */
    size_t max_size = nd_size_t();
    __CPROVER_assume(max_size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, max_size));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, max_size));

    /* allocations */
    ensure_byte_buf_has_allocated_buffer_member(&to);
    ensure_byte_cursor_has_allocated_buffer_member(&from);

    /* validity */
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* save old state */
    struct aws_byte_buf old_to = to;
    struct store_byte_from_buffer old_buffer;
    save_byte_from_array(to.buffer, to.capacity, &old_buffer);

    /* call function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(to.len == old_to.len + from.len);
        assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
    } else {
        assert(result == AWS_ERROR_DEST_COPY_TOO_SMALL);
        assert(to.len == old_to.len);
        assert_byte_from_buffer_matches(to.buffer, &old_buffer);
    }

    /* unchanged fields */
    assert(to.allocator == old_to.allocator);
    assert(to.capacity == old_to.capacity);
    assert(to.buffer == old_to.buffer);

    /* validity invariants */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
