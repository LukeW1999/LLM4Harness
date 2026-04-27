#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_byte_buf dest;
    struct aws_allocator allocator;
    struct aws_byte_buf src;

    /* 2. Bound the structure (limits CBMC state space) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));

    /* 3. Allocate pointer members */
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    ensure_byte_buf_has_allocated_buffer_member(&src);

    /* 4. Assume validity precondition */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* 5. Save old state (for checking immutability) */
    struct aws_byte_buf old_dest = dest;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)dest.buffer, dest.capacity, &old_byte);

    /* 6. Assume function-specific preconditions (from Requires: annotations) */
    __CPROVER_assume(src.buffer != NULL || src.len == 0);
    __CPROVER_assume(allocator.mem_acquire != NULL);

    /* 7. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, &allocator, &src);

    /* 8. Assert postconditions (from Ensures: annotations) */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == &allocator);
        assert(memcmp(dest.buffer, src.buffer, src.len) == 0);
    } else {
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
    }
    assert(aws_byte_buf_is_valid(&dest));  // invariant always holds
    assert_byte_from_buffer_matches((uint8_t *)old_dest.buffer, &old_byte);  // immutability check
}
