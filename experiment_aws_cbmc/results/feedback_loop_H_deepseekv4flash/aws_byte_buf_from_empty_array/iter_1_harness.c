#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_init_copy_harness() {
    /* Parameters */
    struct aws_byte_buf *dest = malloc(sizeof(*dest));
    struct aws_allocator *allocator = nondet_bool() ? malloc(sizeof(*allocator)) : NULL;
    struct aws_byte_buf *src = malloc(sizeof(*src));

    /* Preconditions: allocator and dest are not NULL, src is valid */
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(dest != NULL);
    __CPROVER_assume(src != NULL);
    __CPROVER_assume(aws_byte_buf_is_bounded(src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(src);
    __CPROVER_assume(aws_byte_buf_is_valid(src));

    /* Save old state of src (non-modifiable) */
    struct aws_byte_buf old_src = *src;
    struct store_byte_from_buffer src_buffer_store;
    if (old_src.buffer != NULL && old_src.len > 0) {
        save_byte_from_array(old_src.buffer, old_src.len, &src_buffer_store);
    }

    /* Call function */
    int result = aws_byte_buf_init_copy(dest, allocator, src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        if (old_src.buffer == NULL) {
            /* src had null buffer */
            assert(dest->buffer == NULL);
            assert(dest->len == 0);
            assert(dest->capacity == 0);
            assert(dest->allocator == allocator);
        } else {
            /* src had non-null buffer */
            assert(dest->buffer != NULL);
            assert(dest->len == old_src.len);
            assert(dest->capacity == old_src.capacity);
            assert(dest->allocator == allocator);
            /* Contents match */
            assert(aws_byte_buf_eq(dest, src) || (old_src.len == 0 &&
                   old_src.buffer != NULL && old_src.capacity > 0));
            /* More precise: memory at dest->buffer equals src->buffer for src->len bytes */
            assert_bytes_match(dest->buffer, old_src.buffer, old_src.len);
        }
    } else {
        /* Failure: allocation failed -> dest is zeroed */
        assert(dest->buffer == NULL);
        assert(dest->len == 0);
        assert(dest->capacity == 0);
        assert(dest->allocator == NULL);
    }

    /* src must remain unchanged */
    assert(src->allocator == old_src.allocator);
    assert(src->buffer == old_src.buffer);
    assert(src->len == old_src.len);
    assert(src->capacity == old_src.capacity);
    if (old_src.buffer != NULL && old_src.len > 0) {
        assert_byte_from_buffer_matches(old_src.buffer, &src_buffer_store);
    }

    /* Validity invariants */
    assert(aws_byte_buf_is_valid(dest));
    assert(aws_byte_buf_is_valid(src));
}
