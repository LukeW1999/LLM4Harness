#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;

    __CPROVER_bool zero;
    int result = aws_byte_buf_reset(&buf, zero);

    /* The function should always succeed */
    assert(result == AWS_OP_SUCCESS);

    /* After reset, length must be zero and other fields unchanged */
    assert(buf.len == 0);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);
    assert(buf.buffer == old.buffer);

    /* If zeroing is requested, the original contents must be cleared */
    if (zero) {
        for (size_t i = 0; i < old.len; ++i) {
            assert(buf.buffer[i] == 0);
        }
    }

    /* The buffer must remain valid */
    assert(aws_byte_buf_is_valid(&buf));
}
