#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_secure_zero_harness() {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;

    aws_byte_buf_secure_zero(&buf);

    /* Verify that allocator and capacity are unchanged */
    assert(buf.allocator == old.allocator);
    assert(buf.capacity == old.capacity);

    /* Length should be reset to zero after secure zero */
    assert(buf.len == 0);

    if (buf.buffer != NULL) {
        size_t i;
        for (i = 0; i < buf.capacity; ++i) {
            __CPROVER_assert(buf.buffer[i] == 0, "buffer byte is zeroed");
        }
    } else {
        assert(old.buffer == NULL);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
