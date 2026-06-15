#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_init_copy_harness() {
    /* Declare and bound src */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Declare output buffer */
    struct aws_byte_buf dest;

    /* Declare allocator (must be non-null for allocation) */
    struct aws_allocator *allocator;
    allocator = malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(allocator != NULL);

    /* Save old state of src for immutability check */
    struct aws_byte_buf old_src = src;

    /* Call the function */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - If src->buffer is NULL, dest is zeroed except allocator.
         * - Otherwise, dest is a copy of src with new buffer.
         */
        assert(dest.allocator == allocator);
        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* Compare contents */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }
    } else {
        /* On failure, dest is zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* src must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    /* Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
