#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;

    int zero;
    __CPROVER_assume(zero == 0 || zero == 1);

    aws_byte_buf_reset(&buf, zero);

    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf must be valid after reset");
    __CPROVER_assert(buf.capacity == old.capacity, "capacity unchanged");
    __CPROVER_assert(buf.allocator == old.allocator, "allocator unchanged");
    __CPROVER_assert(buf.buffer == old.buffer, "buffer pointer unchanged");
    __CPROVER_assert(buf.len == 0, "len reset to 0");

    if (zero) {
        size_t i;
        for (i = 0; i < buf.capacity; ++i) {
            __CPROVER_assert(buf.buffer[i] == 0, "buffer zeroed");
        }
    }
}
