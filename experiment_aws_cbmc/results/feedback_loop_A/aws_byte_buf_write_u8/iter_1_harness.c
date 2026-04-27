#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));
    __CPROVER_assume(allocator != NULL);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        if (src.buffer) {
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.buffer == old_dest.buffer);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(dest.allocator == allocator);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
