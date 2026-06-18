#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_from_buf_harness() {
    struct aws_byte_buf buf;

    /* Bound the byte buffer to keep the proof tractable */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Ensure buf has a valid backing buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Assume the byte buffer is valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state for immutability checks */
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_buf(&buf);

    /* Postcondition: the cursor points to the buffer's memory and reports its current length */
    assert(cursor.ptr == buf.buffer);
    assert(cursor.len == buf.len);

    /* The returned cursor must satisfy the validity predicate */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* The original byte_buf is unchanged (the function takes a const pointer) */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.allocator == old_buf.allocator);

    /* The original byte_buf remains valid */
    assert(aws_byte_buf_is_valid(&buf));
}
