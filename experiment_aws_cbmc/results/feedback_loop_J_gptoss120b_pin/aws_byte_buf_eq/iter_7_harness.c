#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_eq_harness() {
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

    int result = aws_byte_buf_eq(&buf1, &buf2);

    /* Compute the expected result according to the specification of aws_byte_buf_eq */
    int expected = 0;
    if (buf1.len == buf2.len) {
        if (buf1.len == 0) {
            expected = 1;
        } else {
            int eq = 1;
            for (size_t i = 0; i < buf1.len; ++i) {
                if (buf1.buffer[i] != buf2.buffer[i]) {
                    eq = 0;
                    break;
                }
            }
            expected = eq;
        }
    }
    __CPROVER_assert(result == expected, "aws_byte_buf_eq result matches expected equality");

    /* Ensure inputs are unchanged */
    __CPROVER_assert(buf1.buffer == old_buf1.buffer, "buf1.buffer unchanged");
    __CPROVER_assert(buf1.len == old_buf1.len, "buf1.len unchanged");
    __CPROVER_assert(buf1.capacity == old_buf1.capacity, "buf1.capacity unchanged");
    __CPROVER_assert(buf1.allocator == old_buf1.allocator, "buf1.allocator unchanged");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf1), "buf1 remains valid");

    __CPROVER_assert(buf2.buffer == old_buf2.buffer, "buf2.buffer unchanged");
    __CPROVER_assert(buf2.len == old_buf2.len, "buf2.len unchanged");
    __CPROVER_assert(buf2.capacity == old_buf2.capacity, "buf2.capacity unchanged");
    __CPROVER_assert(buf2.allocator == old_buf2.allocator, "buf2.allocator unchanged");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf2), "buf2 remains valid");
}
