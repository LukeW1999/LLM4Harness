#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness() {
    /* Data structures */
    struct aws_allocator *allocator = nondet_bool() ? malloc(sizeof(*allocator)) : NULL;
    struct aws_byte_buf *dest = malloc(sizeof(*dest));
    struct aws_byte_buf *src = malloc(sizeof(*src));

    /* Preconditions */
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(dest != NULL);
    __CPROVER_assume(src != NULL);
    __CPROVER_assume(aws_byte_buf_is_bounded(src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(src);
    __CPROVER_assume(aws_byte_buf_is_valid(src));

    /* Save old state of src (since it is const) */
    struct aws_byte_buf old_src = *src;
    struct store_byte_from_buffer old_src_bytes;
    save_byte_from_array(src->buffer, src->len, &old_src_bytes);

    /* Call the function */
    int result = aws_byte_buf_init_copy(dest, allocator, src);

    /* Postconditions */

    /* 1. Validity always holds */
    assert(aws_byte_buf_is_valid(dest));

    /* 2. src is unchanged */
    assert(src->len == old_src.len);
    assert(src->capacity == old_src.capacity);
    assert(src->allocator == old_src.allocator);
    if (src->buffer != NULL && src->len > 0) {
        assert_byte_from_buffer_matches(src->buffer, &old_src_bytes);
    }

    if (result == AWS_OP_SUCCESS) {
        if (old_src.buffer == NULL) {
            /* src->buffer was NULL: dest is zeroed except allocator */
            assert(dest->len == 0);
            assert(dest->capacity == 0);
            assert(dest->buffer == NULL);
            assert(dest->allocator == allocator);
        } else {
            /* Normal copy */
            assert(dest->len == old_src.len);
            assert(dest->capacity == old_src.capacity);
            assert(dest->allocator == allocator);
            assert(dest->buffer != NULL);
            assert_bytes_match(dest->buffer, old_src.buffer, old_src.len);
        }
    } else {
        /* Failure: dest is zeroed */
        assert(dest->len == 0);
        assert(dest->capacity == 0);
        assert(dest->buffer == NULL);
        assert(dest->allocator == NULL);
    }
}
