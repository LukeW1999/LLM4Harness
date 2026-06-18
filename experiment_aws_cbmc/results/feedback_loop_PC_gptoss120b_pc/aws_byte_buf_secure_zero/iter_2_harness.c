#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#   define MAX_BUFFER_SIZE 256
#endif

void aws_byte_buf_secure_zero_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer buf_byte;
    if (buf.buffer != NULL && buf.capacity > 0) {
        save_byte_from_array(buf.buffer, buf.capacity, &buf_byte);
    }

    aws_byte_buf_secure_zero(&buf);

    __CPROVER_assert(buf.len == old_buf.len, "len unchanged");
    __CPROVER_assert(buf.capacity == old_buf.capacity, "capacity unchanged");
    __CPROVER_assert(buf.allocator == old_buf.allocator, "allocator unchanged");
    __CPROVER_assert(buf.buffer == old_buf.buffer, "buffer pointer unchanged");

    if (buf.buffer != NULL && buf.capacity > 0) {
        size_t i;
        for (i = 0; i < buf.capacity; ++i) {
            __CPROVER_assert(buf.buffer[i] == 0, "buffer zeroed");
        }
    }

    __CPROVER_assert(alloc == aws_default_allocator(), "allocator argument unchanged");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf remains valid");
}
