#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = nondet_bool() ? NULL : malloc(sizeof(struct aws_allocator));
    struct aws_byte_buf src;

    /* Preconditions from Doxygen and implementation */
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));
    /* dest is a valid pointer (not necessarily valid buffer) */
    __CPROVER_assume(dest.allocator == NULL); /* dest is uninitialized */

    /* Save old state for immutability checks */
    struct aws_byte_buf old_src = src;

    /* Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, dest is a valid buffer */
        assert(aws_byte_buf_is_valid(&dest));
        /* dest->allocator == allocator */
        assert(dest.allocator == allocator);
        /* dest->len == src->len */
        assert(dest.len == src.len);
        /* dest->capacity == src->len (since src->buffer != NULL) or 0 (if src->buffer == NULL) */
        if (src.buffer != NULL) {
            assert(dest.capacity == src.len);
            /* dest->buffer is allocated and contains copy of src->buffer */
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            assert(dest.capacity == 0);
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
        }
    } else {
        /* On failure, dest is zeroed */
        assert(dest.allocator == NULL);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
    }

    /* src is unchanged */
    assert(src.allocator == old_src.allocator);
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(aws_byte_buf_is_valid(&src));
}
