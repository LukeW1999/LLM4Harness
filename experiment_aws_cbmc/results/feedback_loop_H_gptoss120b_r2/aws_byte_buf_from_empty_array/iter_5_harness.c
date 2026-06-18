#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_init_copy_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest = {0};
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src = src;

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(dest.allocator == allocator, "dest allocator set");
        __CPROVER_assert(src.buffer == old_src.buffer, "src buffer unchanged");
        __CPROVER_assert(src.len == old_src.len, "src len unchanged");
        __CPROVER_assert(src.capacity == old_src.capacity, "src capacity unchanged");
        __CPROVER_assert(src.allocator == old_src.allocator, "src allocator unchanged");
        __CPROVER_assert(aws_byte_buf_is_valid(&dest), "dest valid after success");

        if (src.buffer == NULL) {
            __CPROVER_assert(dest.buffer == NULL, "dest buffer NULL when src NULL");
            __CPROVER_assert(dest.len == 0, "dest len zero when src NULL");
            __CPROVER_assert(dest.capacity == 0, "dest capacity zero when src NULL");
        } else {
            __CPROVER_assert(dest.capacity == src.capacity, "dest capacity matches src");
            __CPROVER_assert(dest.len == src.len, "dest len matches src");
            __CPROVER_assert(dest.buffer != NULL, "dest buffer non‑NULL when src non‑NULL");
            for (size_t i = 0; i < src.len; ++i) {
                __CPROVER_assert(dest.buffer[i] == src.buffer[i],
                                 "byte content matches");
            }
        }
    } else {
        __CPROVER_assert(dest.buffer == NULL, "dest buffer NULL on failure");
        __CPROVER_assert(dest.len == 0, "dest len zero on failure");
        __CPROVER_assert(dest.capacity == 0, "dest capacity zero on failure");
        __CPROVER_assert(dest.allocator == NULL, "dest allocator NULL on failure");
        __CPROVER_assert(aws_byte_buf_is_valid(&dest), "dest valid after failure");
        __CPROVER_assert(src.buffer == old_src.buffer, "src buffer unchanged on failure");
        __CPROVER_assert(src.len == old_src.len, "src len unchanged on failure");
        __CPROVER_assert(src.capacity == old_src.capacity, "src capacity unchanged on failure");
        __CPROVER_assert(src.allocator == old_src.allocator, "src allocator unchanged on failure");
    }
}
