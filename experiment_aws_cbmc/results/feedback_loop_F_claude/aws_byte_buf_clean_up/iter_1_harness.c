#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_byte_buf_clean_up
 *
 * From the implementation:
 * - Precondition: aws_byte_buf_is_valid(buf)
 * - If buf->allocator && buf->buffer, releases the buffer memory
 * - Sets buf->allocator = NULL
 * - Sets buf->buffer = NULL
 * - Sets buf->len = 0
 * - Sets buf->capacity = 0
 * - Postcondition: buf is a zeroed-out valid aws_byte_buf
 */
void aws_byte_buf_clean_up_harness() {
    struct aws_byte_buf buf;

    /* Bound the buffer size to keep verification tractable */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate the buffer member non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Assume the buffer is valid before calling */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Call the function under test */
    aws_byte_buf_clean_up(&buf);

    /* Postconditions: all fields must be zeroed out */
    assert(buf.allocator == NULL);
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    /* Validity invariant must hold after the call */
    assert(aws_byte_buf_is_valid(&buf));
}
