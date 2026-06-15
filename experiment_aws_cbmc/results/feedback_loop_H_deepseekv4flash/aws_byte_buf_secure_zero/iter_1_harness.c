#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf *dest = malloc(sizeof(struct aws_byte_buf));
    struct aws_allocator *allocator = malloc(sizeof(struct aws_allocator));
    struct aws_byte_buf *src = malloc(sizeof(struct aws_byte_buf));

    /* Preconditions: pointers are non-null */
    __CPROVER_assume(dest != NULL && allocator != NULL && src != NULL);

    /* Ensure src is a valid aws_byte_buf */
    __CPROVER_assume(aws_byte_buf_is_valid(src));

    /* Save old state for immutability checks */
    struct aws_byte_buf old_src = *src;

    /* Call the function */
    int result = aws_byte_buf_init_copy(dest, allocator, src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: dest->len == src->len */
        assert(dest->len == src->len);
        /* On success: dest->capacity == src->len */
        assert(dest->capacity == src->len);
        /* On success: dest->allocator == allocator */
        assert(dest->allocator == allocator);
        if (src->buffer == NULL) {
            /* If src buffer is null, dest should have null buffer, len 0, cap 0 */
            assert(dest->buffer == NULL);
            assert(dest->len == 0);
            assert(dest->capacity == 0);
        } else {
            /* Otherwise dest buffer is non-null and allocated memory */
            assert(dest->buffer != NULL);
            /* Ensure the contents match */
            assert_bytes_match(dest->buffer, src->buffer, src->len);
        }
        /* dest is a valid aws_byte_buf */
        assert(aws_byte_buf_is_valid(dest));
    } else {
        /* On failure: dest is zeroed */
        assert(dest->buffer == NULL);
        assert(dest->len == 0);
        assert(dest->capacity == 0);
        assert(dest->allocator == NULL);
    }

    /* Immutability: src must remain unchanged */
    assert(src->len == old_src.len);
    assert(src->capacity == old_src.capacity);
    assert(src->allocator == old_src.allocator);
    if (src->buffer != NULL) {
        assert(src->buffer == old_src.buffer);
    }
    /* src remains valid */
    assert(aws_byte_buf_is_valid(src));
}
