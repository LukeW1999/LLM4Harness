#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_buf_harness() {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;

    struct aws_byte_cursor cursor = aws_byte_cursor_from_buf(&buf);

    /* Postconditions: returned cursor matches buf */
    assert(cursor.ptr == buf.buffer);
    assert(cursor.len == buf.len);
    assert(aws_byte_cursor_is_valid(&cursor));

    /* Immutability: buf is passed as const and must remain unchanged */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    
    /* Validity invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
