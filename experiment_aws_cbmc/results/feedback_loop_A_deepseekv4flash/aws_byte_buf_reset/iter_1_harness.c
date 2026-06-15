#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_init_copy_harness() {
    /* Non-deterministic parameters */
    struct aws_byte_buf *dest = malloc(sizeof(*dest));
    struct aws_allocator *allocator = nondet_bool() ? NULL : malloc(sizeof(struct aws_allocator));
    struct aws_byte_buf *src = malloc(sizeof(*src));

    /* Preconditions according to the function and */
    /* we assume non-null pointers for dest and allocator */
    __CPROVER_assume(dest != NULL);
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(src != NULL);

    /* Bound src buffer size to keep analysis tractable */
    __CPROVER_assume(aws_byte_buf_is_bounded(src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(src);
    __CPROVER_assume(aws_byte_buf_is_valid(src));

    /* Save old state of src to check immutability */
    struct aws_byte_buf old_src = *src;

    /* Call the function */
    int result = aws_byte_buf_init_copy(dest, allocator, src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success case */
        assert(aws_byte_buf_is_valid(dest));
        assert(dest->allocator == allocator);

        if (src->buffer == NULL) {
            /* src empty: dest should be zeroed except allocator set */
            assert(dest->buffer == NULL);
            assert(dest->len == 0);
            assert(dest->capacity == 0);
        } else {
            /* src non-empty: dest should have allocated buffer,
               len = src->len, capacity = src->len, and contents equal */
            assert(dest->buffer != NULL);
            assert(dest->len == src->len);
            assert(dest->capacity == src->len);
            /* Memory allocated must be writable and have correct contents */
            __CPROVER_assume(dest->buffer != NULL);
            assert_bytes_match(dest->buffer, src->buffer, dest->len);
        }
    } else {
        /* Failure case: only possible when src non-NULL and allocation fails */
        assert(result == AWS_OP_ERR);
        assert(src->buffer != NULL); /* failure only if src had content */
        /* On failure, dest is zeroed completely */
        assert(dest->buffer == NULL);
        assert(dest->len == 0);
        assert(dest->capacity == 0);
        /* allocator is not set on failure */
        /* (it may be any value because AWS_ZERO_STRUCT sets all to 0) */
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

    /* Clean up allocated memory to avoid leaks */
    free(src);
    free(dest);
    free(allocator);
}
