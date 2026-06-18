#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_clean_up_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_allocator *old_allocator = buf.allocator;

    aws_byte_buf_clean_up(&buf);

    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    /* allocator may be unchanged or set to NULL after clean_up */
    assert(buf.allocator == old_allocator || buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
