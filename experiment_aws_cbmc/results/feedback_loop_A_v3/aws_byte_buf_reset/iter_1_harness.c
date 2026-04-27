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

    /* 5. Save old state (for checking immutability) */
    struct aws_byte_buf old_dest = dest;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)dest.buffer, dest.capacity, &old_byte);

    /* 6. Assume function-specific preconditions (from Requires: annotations) */
    __CPROVER_assume(&src && &allocator);

    /* 7. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, &allocator, &src);

    /* 8. Assert postconditions (from Ensures: annotations) */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == &allocator);
        assert_bytes_match(dest.buffer, src.buffer, src.len);
    } else if (result == AWS_OP_ERR) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == &allocator);
    }
    assert(aws_byte_buf_is_valid(&dest));  // invariant always holds
}
