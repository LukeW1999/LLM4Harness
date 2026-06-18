#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_init_copy_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf old_src = src;

    struct aws_byte_buf dest;

    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    __CPROVER_assert(aws_byte_buf_is_valid(&dest), "dest must be valid");
    __CPROVER_assert(aws_byte_buf_is_valid(&src), "src must be valid");

    __CPROVER_assert(src.len == old_src.len, "src.len unchanged");
    __CPROVER_assert(src.capacity == old_src.capacity, "src.capacity unchanged");
    __CPROVER_assert(src.buffer == old_src.buffer, "src.buffer unchanged");
    __CPROVER_assert(src.allocator == old_src.allocator, "src.allocator unchanged");

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(dest.allocator == alloc, "dest.allocator set correctly");
        __CPROVER_assert(dest.len == src.len, "dest.len matches src.len");
        __CPROVER_assert(dest.capacity == src.capacity, "dest.capacity matches src.capacity");

        if (src.capacity == 0) {
            __CPROVER_assert(dest.buffer == NULL, "dest.buffer is NULL when src.capacity is 0");
        } else {
            __CPROVER_assert(dest.buffer != NULL, "dest.buffer is non-NULL when src.capacity > 0");
        }

        if (src.len > 0) {
            size_t i;
            for (i = 0; i < src.len; ++i) {
                __CPROVER_assert(dest.buffer[i] == src.buffer[i],
                                 "dest buffer byte matches src buffer byte");
            }
        }
    } else {
        __CPROVER_assert(dest.buffer == NULL, "dest.buffer is NULL on failure");
        __CPROVER_assert(dest.len == 0, "dest.len is 0 on failure");
        __CPROVER_assert(dest.capacity == 0, "dest.capacity is 0 on failure");
        __CPROVER_assert(dest.allocator == NULL, "dest.allocator is NULL on failure");
    }
}
