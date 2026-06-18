#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_reset_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf buf;
    buf.allocator = allocator;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;

    uint8_t old_contents[MAX_BUFFER_SIZE];
    if (buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; ++i) {
            old_contents[i] = buf.buffer[i];
        }
    }

    bool zero_fill = __CPROVER_nondet_bool();

    int result = aws_byte_buf_reset(&buf, zero_fill);
    __CPROVER_assert(result == AWS_OP_SUCCESS, "aws_byte_buf_reset should succeed");

    __CPROVER_assert(buf.len == 0, "buf.len should be reset to zero");
    __CPROVER_assert(buf.capacity == old_buf.capacity, "buf.capacity should remain unchanged");
    __CPROVER_assert(buf.allocator == old_buf.allocator, "buf.allocator should remain unchanged");
    __CPROVER_assert(buf.buffer == old_buf.buffer, "buf.buffer pointer should remain unchanged");

    if (zero_fill && buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; ++i) {
            __CPROVER_assert(buf.buffer[i] == 0, "buffer should be zeroed after reset");
        }
    } else if (buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; ++i) {
            __CPROVER_assert(buf.buffer[i] == old_contents[i],
                             "buffer contents should be unchanged when zero_fill is false");
        }
    }

    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf should remain valid after reset");
}
