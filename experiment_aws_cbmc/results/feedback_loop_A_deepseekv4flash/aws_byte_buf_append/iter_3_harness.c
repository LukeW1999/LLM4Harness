#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"
#include "aws/common/byte_buf.h"

void aws_byte_buf_append_harness() {
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    ensure_byte_buf_has_allocated_buffer_member(&to);
    ensure_byte_cursor_has_allocated_buffer_member(&from);

    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    // Nondeterministically decide if capacity is sufficient
    bool can_append;
    __CPROVER_assume(can_append == (to.capacity - to.len >= from.len));

    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;
    struct store_byte_from_buffer old_to_buffer;
    save_byte_from_array(to.buffer, to.len, &old_to_buffer);

    int result = aws_byte_buf_append(&to, &from);

    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
    assert(to.allocator == old_to.allocator);
    assert(to.capacity == old_to.capacity);
    assert(to.buffer == old_to.buffer);
    assert(from.ptr == old_from.ptr);
    assert(from.len == old_from.len);

    if (result == AWS_OP_SUCCESS) {
        assert(can_append);
        assert(to.len == old_to.len + from.len);
        assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
    } else {
        assert(!can_append);
        assert(to.len == old_to.len);
        assert_byte_from_buffer_matches(to.buffer, &old_to_buffer);
    }
}
