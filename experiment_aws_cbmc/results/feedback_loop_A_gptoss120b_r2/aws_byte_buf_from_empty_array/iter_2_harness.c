#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_copy_harness(void) {
    /* allocator used for the operation */
    struct aws_allocator *allocator = aws_default_allocator();

    /* source buffer (input) */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* destination buffer (output) */
    struct aws_byte_buf dest;

    /* Save old state of src for immutability checks */
    struct aws_byte_buf old_src = src;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* src must remain unchanged regardless of the result */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* The function always sets dest.allocator to the supplied allocator */
    assert(dest.allocator == allocator);

    if (result == AWS_OP_SUCCESS) {
        if (src.buffer == NULL) {
            /* When src has no buffer, dest is zeroed (len/capacity 0, buffer NULL) */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* Successful allocation and copy */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* The contents of the new buffer must match the source up to src.len */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* Allocation failure: dest is zeroed (except allocator) */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
    }

    /* In all cases the resulting buffer must be valid */
    assert(aws_byte_buf_is_valid(&dest));
}
