#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    struct aws_byte_buf src;

    /* Bounding and preconditions from Doxygen and header */
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old state of dest and src to check immutability */
    struct aws_byte_buf old_dest;
    struct aws_byte_buf old_src = src;

    /* Call function */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, dest is a copy of src with new allocator */
        if (src.buffer != NULL) {
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.allocator == allocator);
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);  // Must be a different allocation
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.buffer == NULL);
            assert(dest.allocator == allocator);
        }
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* On failure, dest is zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* src must remain unchanged */
    assert_bytes_match(src.buffer, old_src.buffer, src.len);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(src.buffer == old_src.buffer);
    assert(aws_byte_buf_is_valid(&src));
}
