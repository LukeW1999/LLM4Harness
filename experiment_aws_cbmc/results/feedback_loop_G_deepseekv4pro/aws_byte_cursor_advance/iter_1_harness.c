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
    struct aws_byte_buf *src = malloc(sizeof(*src));
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(dest != NULL);
    __CPROVER_assume(src != NULL);

    /* Bound src */
    __CPROVER_assume(aws_byte_buf_is_bounded(src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(src);
    __CPROVER_assume(aws_byte_buf_is_valid(src));

    /* Save old state for immutability checks */
    struct aws_byte_buf old_src = *src;

    /* 2. Call function under test */
    int result = aws_byte_buf_init_copy(dest, allocator, src);

    /* 3. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Doxygen: "Copies src buffer into dest and sets the correct len and capacity." */
        /* Doxygen: "Dest capacity and len will be equal to the src len." */
        assert(dest->len == src->len);
        assert(dest->capacity == src->len);

        /* Doxygen: "A new memory zone is allocated for dest->buffer." */
        /* Doxygen: "Allocator of the dest will be identical with parameter allocator." */
        assert(dest->allocator == allocator);

        /* Doxygen: "If src buffer is null the dest will have a null buffer with a len and a capacity of 0" */
        if (src->buffer == NULL) {
            assert(dest->buffer == NULL);
            assert(dest->len == 0);
            assert(dest->capacity == 0);
        } else {
            /* Doxygen: "A new memory zone is allocated for dest->buffer." */
            assert(dest->buffer != NULL);
            /* Contents should match */
            assert_bytes_match(dest->buffer, src->buffer, src->len);
        }
    } else {
        /* Doxygen: "Returns AWS_OP_ERR when memory can't be allocated." */
        assert(result == AWS_OP_ERR);
        /* On failure, dest should be zeroed out */
        assert(dest->buffer == NULL);
        assert(dest->len == 0);
        assert(dest->capacity == 0);
        assert(dest->allocator == NULL);
    }

    /* 4. Assert src is unchanged */
    assert(src->buffer == old_src.buffer);
    assert(src->len == old_src.len);
    assert(src->capacity == old_src.capacity);
    assert(src->allocator == old_src.allocator);

    /* 5. Assert validity invariants */
    assert(aws_byte_buf_is_valid(dest));
    assert(aws_byte_buf_is_valid(src));

    /* Cleanup */
    free(dest);
    free(src);
}
