#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf *dest = malloc(sizeof(*dest));
    struct aws_allocator *allocator = malloc(sizeof(*allocator));
    struct aws_byte_buf *src = malloc(sizeof(*src));

    /* Precondition: pointers not NULL */
    __CPROVER_assume(dest != NULL);
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(src != NULL);

    /* Precondition: src is a valid byte buffer */
    __CPROVER_assume(aws_byte_buf_is_valid(src));

    /* Save old state of src (immutability check) */
    struct aws_byte_buf old_src = *src;
    struct store_byte_from_buffer src_storage;
    if (src->buffer && src->len > 0) {
        save_byte_from_array(src->buffer, src->len, &src_storage);
    }

    /* Call the function */
    int result = aws_byte_buf_init_copy(dest, allocator, src);

    /* Basic reachable assertion */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERROR);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: dest reflects src contents */
        assert(dest->allocator == allocator);
        assert(dest->len == src->len);
        assert(dest->capacity == src->capacity);
        if (src->buffer) {
            assert(dest->buffer != NULL);
            assert(dest->capacity > 0);
            assert(dest->len <= dest->capacity);
            assert_bytes_match(dest->buffer, src->buffer, src->len);
        } else {
            assert(dest->buffer == NULL);
            assert(dest->len == 0);
            assert(dest->capacity == 0);
        }
    } else {
        /* Failure: dest is zeroed completely */
        assert(dest->buffer == NULL);
        assert(dest->len == 0);
        assert(dest->capacity == 0);
        assert(dest->allocator == NULL);
    }

    /* Unchanged fields of src (immutable) */
    assert(src->allocator == old_src.allocator);
    assert(src->len == old_src.len);
    assert(src->capacity == old_src.capacity);
    if (src->buffer && src->len > 0) {
        assert_byte_from_buffer_matches(src->buffer, &src_storage);
    }

    /* Validity invariants */
    assert(aws_byte_buf_is_valid(dest));
    assert(aws_byte_buf_is_valid(src));

    /* Cleanup (optional for CBMC) */
    if (dest->buffer) free(dest->buffer);
    free(dest);
    if (src->buffer) free(src->buffer);
    free(src);
    free(allocator);
}
