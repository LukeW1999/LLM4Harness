#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    struct aws_byte_buf dest;
    struct aws_byte_buf old_dest = dest;

    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));
    struct aws_byte_buf old_src = src;

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    if (src.buffer == NULL) {
        __CPROVER_assert(result == AWS_OP_SUCCESS, "result should be success");
        __CPROVER_assert(dest.buffer == NULL, "dest.buffer should be NULL");
        __CPROVER_assert(dest.len == 0, "dest.len should be 0");
        __CPROVER_assert(dest.capacity == 0, "dest.capacity should be 0");
        __CPROVER_assert(dest.allocator == allocator, "dest.allocator should match allocator");
        __CPROVER_assert(aws_byte_buf_is_valid(&dest), "dest should be valid");
    } else {
        if (result == AWS_OP_SUCCESS) {
            __CPROVER_assert(dest.buffer != NULL, "dest.buffer should not be NULL");
            __CPROVER_assert(dest.capacity == src.capacity, "dest.capacity should match src.capacity");
            __CPROVER_assert(dest.len == src.len, "dest.len should match src.len");
            __CPROVER_assert(dest.allocator == allocator, "dest.allocator should match allocator");
            assert_bytes_match(dest.buffer, src.buffer, src.len);
            __CPROVER_assert(aws_byte_buf_is_valid(&dest), "dest should be valid");
        } else {
            __CPROVER_assert(result == AWS_OP_ERR, "result should be error");
            __CPROVER_assert(dest.buffer == NULL, "dest.buffer should be NULL on error");
            __CPROVER_assert(dest.len == 0, "dest.len should be 0 on error");
            __CPROVER_assert(dest.capacity == 0, "dest.capacity should be 0 on error");
            __CPROVER_assert(dest.allocator == NULL, "dest.allocator should be NULL on error");
            __CPROVER_assert(aws_byte_buf_is_valid(&dest), "dest should be valid");
        }
    }

    __CPROVER_assert(src.buffer == old_src.buffer, "src.buffer unchanged");
    __CPROVER_assert(src.len == old_src.len, "src.len unchanged");
    __CPROVER_assert(src.capacity == old_src.capacity, "src.capacity unchanged");
    __CPROVER_assert(src.allocator == old_src.allocator, "src.allocator unchanged");
    __CPROVER_assert(aws_byte_buf_is_valid(&src), "src should be valid");
}
