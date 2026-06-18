#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf old_src = src;

    struct aws_byte_buf dest;
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;
    dest.allocator = NULL;

    struct aws_allocator *allocator = aws_default_allocator();

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    __CPROVER_assert(aws_byte_buf_is_valid(&dest), "dest must be valid");

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(dest.allocator == allocator, "allocator set correctly");

        if (src.buffer == NULL) {
            __CPROVER_assert(dest.buffer == NULL, "dest buffer is NULL when src.buffer is NULL");
            __CPROVER_assert(dest.len == 0, "dest len is 0 when src.buffer is NULL");
            __CPROVER_assert(dest.capacity == 0, "dest capacity is 0 when src.buffer is NULL");
        } else {
            __CPROVER_assert(dest.buffer != NULL, "dest.buffer is non‑NULL when src.buffer is non‑NULL");
            __CPROVER_assert(dest.len == src.len, "dest.len equals src.len");
            __CPROVER_assert(dest.capacity == src.capacity, "dest.capacity equals src.capacity");
            for (size_t i = 0; i < src.len; ++i) {
                __CPROVER_assert(dest.buffer[i] == src.buffer[i], "bytes match");
            }
        }

        __CPROVER_assert(src.buffer == old_src.buffer, "src.buffer unchanged");
        __CPROVER_assert(src.len == old_src.len, "src.len unchanged");
        __CPROVER_assert(src.capacity == old_src.capacity, "src.capacity unchanged");
        __CPROVER_assert(src.allocator == old_src.allocator, "src.allocator unchanged");
        __CPROVER_assert(aws_byte_buf_is_valid(&src), "src remains valid");
    } else {
        __CPROVER_assert(dest.buffer == NULL, "dest.buffer is NULL on failure");
        __CPROVER_assert(dest.len == 0, "dest.len is 0 on failure");
        __CPROVER_assert(dest.capacity == 0, "dest.capacity is 0 on failure");
        __CPROVER_assert(dest.allocator == NULL, "dest.allocator is NULL on failure");

        __CPROVER_assert(src.buffer == old_src.buffer, "src.buffer unchanged");
        __CPROVER_assert(src.len == old_src.len, "src.len unchanged");
        __CPROVER_assert(src.capacity == old_src.capacity, "src.capacity unchanged");
        __CPROVER_assert(src.allocator == old_src.allocator, "src.allocator unchanged");
        __CPROVER_assert(aws_byte_buf_is_valid(&src), "src remains valid");
    }
}
