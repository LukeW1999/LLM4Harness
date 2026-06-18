#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;
    bool zero_fill = nondet_bool();

    aws_byte_buf_reset(&buf, zero_fill);

    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);
    assert(buf.buffer == old.buffer);

    if (zero_fill && old.buffer != NULL) {
        size_t i;
        for (i = 0; i < old.len; ++i) {
            assert(buf.buffer[i] == 0);
        }
    }
}
