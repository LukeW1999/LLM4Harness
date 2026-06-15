#include <aws/common/byte_buf.h>
#include <aws/common/private/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_init_copy_harness() {
    /* Non-deterministic parameters */
    struct aws_byte_buf *dest = malloc(sizeof(*dest));
    struct aws_allocator *allocator = malloc(sizeof(*allocator));
    struct aws_byte_buf *src = malloc(sizeof(*src));

    /* Preconditions: pointers non-null */
    __CPROVER_assume(dest != NULL);
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(src != NULL);

    /* Bound the size of src to limit state space */
    __CPROVER_assume(aws_byte_buf_is_bounded(src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(src);
    __CPROVER_assume(aws_byte_buf_is_valid(src));

    /* Save old state of src for immutability check (src should not be modified) */
    struct aws_byte_buf old_src = *src;

    /* Save old allocator pointer (used to check dest->allocator set correctly) */
    struct aws_allocator *old_allocator = allocator;

    /* Call the function */
    int result = aws_byte_buf_init_copy(dest, allocator, src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: dest is valid */
        assert(aws_byte_buf_is_valid(dest));

        /* dest->allocator must be the provided allocator */
        assert(dest->allocator == allocator);

        /* dest->len must equal src->len */
        assert(dest->len == src->len);

        /* dest->capacity must equal src->capacity */
        assert(dest->capacity == src->capacity);

        /* If src->buffer was NULL, then dest->buffer must be NULL and len/capacity 0 */
        if (src->buffer == NULL) {
            assert(dest->buffer == NULL);
            assert(dest->len == 0);
            assert(dest->capacity == 0);
        } else {
            /* Otherwise, dest->buffer should be non-NULL and contain a copy of src's data */
            assert(dest->buffer != NULL);
            assert_bytes_match(dest->buffer, src->buffer, src->len);
        }
    } else {
        /* Failure: dest must be zeroed */
        assert(dest->buffer == NULL);
        assert(dest->len == 0);
        assert(dest->capacity == 0);
        assert(dest->allocator == NULL);
        /* dest may be invalid on failure, but we keep it zeroed */
    }

    /* src must remain unchanged */
    assert(src->len == old_src.len);
    assert(src->capacity == old_src.capacity);
    assert(src->allocator == old_src.allocator);
    if (src->buffer != NULL) {
        assert_bytes_match(src->buffer, old_src.buffer, src->len);
    } else {
        assert(src->buffer == old_src.buffer);
    }

    /* Free allocated memory (for CBMC, not strictly necessary) */
    free(dest);
    free(allocator);
    free(src);
}
