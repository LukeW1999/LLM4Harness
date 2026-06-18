#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf *dest = malloc(sizeof(*dest));
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_buf *src = malloc(sizeof(*src));

    /* Ensure pointers are valid */
    __CPROVER_assume(dest != NULL);
    __CPROVER_assume(src != NULL);

    /* Bound and initialize src */
    __CPROVER_assume(aws_byte_buf_is_bounded(src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(src);
    __CPROVER_assume(aws_byte_buf_is_valid(src));

    /* Save old state of src for immutability check */
    struct aws_byte_buf old_src = *src;
    struct store_byte_from_buffer src_storage;
    if (src->buffer != NULL && src->capacity > 0) {
        save_byte_from_array(src->buffer, src->capacity, &src_storage);
    }

    /* 2. Call function under test */
    int result = aws_byte_buf_init_copy(dest, allocator, src);

    /* 3. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* From Doxygen: "Copies src buffer into dest and sets the correct len and capacity.
         * A new memory zone is allocated for dest->buffer.
         * Dest capacity and len will be equal to the src len.
         * Allocator of the dest will be identical with parameter allocator.
         * If src buffer is null the dest will have a null buffer with a len and a capacity of 0" */
        if (src->buffer == NULL) {
            assert(dest->buffer == NULL);
            assert(dest->len == 0);
            assert(dest->capacity == 0);
        } else {
            assert(dest->buffer != NULL);
            assert(dest->len == src->len);
            assert(dest->capacity == src->capacity);
            /* Verify contents were copied */
            assert_bytes_match(dest->buffer, src->buffer, src->len);
        }
        assert(dest->allocator == allocator);
    } else {
        /* On failure: memory allocation failed.
         * From implementation: dest is zeroed out on failure. */
        assert(dest->buffer == NULL);
        assert(dest->len == 0);
        assert(dest->capacity == 0);
        assert(dest->allocator == NULL);
    }

    /* 4. Assert src is unchanged (immutability) */
    assert(src->len == old_src.len);
    assert(src->capacity == old_src.capacity);
    assert(src->allocator == old_src.allocator);
    assert(src->buffer == old_src.buffer);
    if (src->buffer != NULL && src->capacity > 0) {
        assert_byte_from_buffer_matches(src->buffer, &src_storage);
    }

    /* 5. Assert validity invariants always hold */
    assert(aws_byte_buf_is_valid(dest));
    assert(aws_byte_buf_is_valid(src));

    /* Cleanup */
    free(dest);
    free(src);
}
