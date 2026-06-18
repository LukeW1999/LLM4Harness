#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>

#define MAX_BUFFER_SIZE 1024U

void aws_byte_buf_clean_up_secure_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf buf;
    buf.allocator = allocator;

    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, (size_t)MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;

    struct store_byte_from_buffer old_byte;
    if (old.buffer != NULL && old.capacity > 0) {
        save_byte_from_array(old.buffer, old.capacity, &old_byte);
    }

    aws_byte_buf_clean_up_secure(&buf);

    __CPROVER_assert(buf.buffer == NULL, "buf.buffer is NULL");
    __CPROVER_assert(buf.len == 0, "buf.len is 0");
    __CPROVER_assert(buf.capacity == 0, "buf.capacity is 0");
    __CPROVER_assert(buf.allocator == old.allocator, "buf.allocator unchanged");

    if (old.buffer != NULL && old.capacity > 0) {
        __CPROVER_assert(old.buffer[old_byte.idx] == 0, "old buffer cleared");
    }

    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf is valid");
}
