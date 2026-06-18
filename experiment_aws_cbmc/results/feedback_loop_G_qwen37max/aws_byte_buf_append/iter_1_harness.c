#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_append_harness() {
    struct aws_byte_buf to;
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&to);
    __CPROVER_assume(aws_byte_buf_is_valid(&to));

    struct aws_byte_cursor from;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&from);
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    struct store_byte_from_buffer stored_byte;
    if (from.len > 0) {
        save_byte_from_array(from.ptr, from.len, &stored_byte);
    }

    int result = aws_byte_buf_append(&to, &from);

    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));

    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    if (result == AWS_OP_SUCCESS) {
        assert(to.len == old_to.len + from.len);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert(to.buffer == old_to.buffer);
        if (from.len > 0) {
            assert_byte_from_buffer_matches(to.buffer + old_to.len, &stored_byte);
        }
    } else {
        assert(to.len == old_to.len);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert(to.buffer == old_to.buffer);
    }
}
