#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE (1024)
#endif

void aws_byte_buf_reset_harness() {
    struct aws_byte_buf buf;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    uint8_t *old_buffer = buf.buffer;
    size_t old_capacity = buf.capacity;
    struct aws_allocator *old_allocator = buf.allocator;

    bool zero_contents;
    aws_byte_buf_reset(&buf, zero_contents);

    /* Post-conditions */
    assert(buf.len == 0);
    assert(buf.capacity == old_capacity);
    assert(buf.buffer == old_buffer);
    assert(buf.allocator == old_allocator);
    assert(aws_byte_buf_is_valid(&buf));

    if (zero_contents && buf.buffer != NULL) {
        for (size_t i = 0; i < buf.capacity; i++) {
            assert(buf.buffer[i] == 0);
        }
    }
}
