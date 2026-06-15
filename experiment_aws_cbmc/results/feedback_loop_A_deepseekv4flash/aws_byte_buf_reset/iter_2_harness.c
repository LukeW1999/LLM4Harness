#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_init_copy_harness() {
    /* Non-deterministic parameters */
    struct aws_byte_buf *dest = malloc(sizeof(*dest));
    struct aws_allocator *allocator = malloc(sizeof(struct aws_allocator));
    struct aws_byte_buf *src = malloc(sizeof(*src));

    /* Preconditions */
    __CPROVER_assume(dest != NULL);
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(src != NULL);

    /* Bound src buffer size */
    __CPROVER_assume(aws_byte_buf_is_bounded(src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(src);
    __CPROVER_assume(aws_byte_buf_is_valid(src));

    /* Save old state */
    struct aws_byte_buf old_src = *src;

    /* Call the function */
    int result = aws_byte_buf_init_copy(dest, allocator, src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(dest));
        assert(dest->allocator == allocator);
        if (src->buffer == NULL) {
            assert(dest->buffer == NULL);
            assert(dest->len == 0);
            assert(dest->capacity == 0);
        } else {
            assert(dest->buffer != NULL);
            assert(dest->len == src->len);
            assert(dest->capacity == src->len);
            assert_bytes_match(dest->buffer, src->buffer, dest->len);
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(dest->buffer == NULL);
        assert(dest->len == 0);
        assert(dest->capacity == 0);
        assert(dest->allocator == NULL);
    }

    /* src must remain unchanged */
    assert(aws_byte_buf_is_valid(src));
    assert(src->allocator == old_src.allocator);
    assert(src->buffer == old_src.buffer);
    assert(src->len == old_src.len);
    assert(src->capacity == old_src.capacity);
    if (src->buffer != NULL) {
        assert_bytes_match(src->buffer, old_src.buffer, src->len);
    }

    /* Clean up */
    free(src);
    free(dest);
    free(allocator);
}
