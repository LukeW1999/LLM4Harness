#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_append_harness() {
    struct aws_byte_buf *to = malloc(sizeof(struct aws_byte_buf));
    struct aws_byte_cursor *from = malloc(sizeof(struct aws_byte_cursor));
    __CPROVER_assume(aws_byte_buf_is_bounded(to, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(from, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(to);
    ensure_byte_cursor_has_allocated_buffer_member(from);
    __CPROVER_assume(aws_byte_buf_is_valid(to));
    __CPROVER_assume(aws_byte_cursor_is_valid(from));

    struct aws_byte_buf old_to = *to;
    struct aws_byte_cursor old_from = *from;

    int result = aws_byte_buf_append(to, from);

    if (result == AWS_OP_SUCCESS) {
        assert(to->len == old_to.len + from->len);
        assert(to->buffer == old_to.buffer || to->buffer != old_to.buffer);
    } else {
        assert(to->len == old_to.len);
        assert(to->buffer == old_to.buffer);
        assert(to->capacity == old_to.capacity);
        assert(to->allocator == old_to.allocator);
    }

    assert(to->allocator == old_to.allocator);
    assert(from->len == old_from.len);
    assert(from->ptr == old_from.ptr);

    assert(aws_byte_buf_is_valid(to));
    assert(aws_byte_cursor_is_valid(from));
}
