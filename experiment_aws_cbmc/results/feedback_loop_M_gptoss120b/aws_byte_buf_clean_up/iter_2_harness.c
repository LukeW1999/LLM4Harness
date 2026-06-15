#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_clean_up_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;

    aws_byte_buf_clean_up(&buf);

    /* After clean_up, fields should be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));

    (void)old_buf;
}
