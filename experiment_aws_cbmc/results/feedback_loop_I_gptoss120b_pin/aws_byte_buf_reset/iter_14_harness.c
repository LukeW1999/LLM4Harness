#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();
    buf.allocator = allocator;

    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;

    _Bool zero = nondet_bool();
    int result = aws_byte_buf_reset(&buf, zero);

    __CPROVER_assert(result == AWS_OP_SUCCESS, "reset should succeed");
    __CPROVER_assert(buf.len == 0, "length should be zero after reset");
    __CPROVER_assert(buf.capacity == old.capacity, "capacity unchanged");
    __CPROVER_assert(buf.allocator == old.allocator, "allocator unchanged");
    __CPROVER_assert(buf.buffer == old.buffer, "buffer pointer unchanged");

    if (zero) {
        for (size_t i = 0; i < old.len; ++i) {
            __CPROVER_assert(buf.buffer[i] == 0, "buffer zeroed");
        }
    }

    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buffer remains valid");
}
