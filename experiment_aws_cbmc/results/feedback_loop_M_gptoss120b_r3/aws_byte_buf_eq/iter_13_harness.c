#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void aws_byte_buf_eq_harness(void) {
    struct aws_byte_buf buf1;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf1);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf1));

    struct aws_byte_buf buf2;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf2, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf2);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf2));

    struct aws_byte_buf old_buf1 = buf1;
    struct aws_byte_buf old_buf2 = buf2;

    int eq = aws_byte_buf_eq(&buf1, &buf2);

    /* Verify that inputs are unchanged */
    assert(buf1.buffer == old_buf1.buffer);
    assert(buf1.len == old_buf1.len);
    assert(buf1.capacity == old_buf1.capacity);
    assert(buf1.allocator == old_buf1.allocator);

    assert(buf2.buffer == old_buf2.buffer);
    assert(buf2.len == old_buf2.len);
    assert(buf2.capacity == old_buf2.capacity);
    assert(buf2.allocator == old_buf2.allocator);

    /* Result must be boolean */
    assert(eq == 0 || eq == 1);
}
