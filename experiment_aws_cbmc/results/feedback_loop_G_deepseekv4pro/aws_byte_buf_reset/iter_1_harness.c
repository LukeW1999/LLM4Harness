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
    struct aws_byte_buf *dest;
    struct aws_allocator *allocator;
    struct aws_byte_buf *src;

    /* Allocate and initialize structures */
    dest = malloc(sizeof(*dest));
    allocator = aws_default_allocator();
    src = malloc(sizeof(*src));

    /* Bound src */
    __CPROVER_assume(aws_byte_buf_is_bounded(src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(src);
    __CPROVER_assume(aws_byte_buf_is_valid(src));

    /* 2. Save old state BEFORE calling */
    struct aws_byte_buf old_src = *src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(dest, allocator, src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* From Doxygen: "Dest capacity and len will be equal to the src len." */
        assert(dest->len == src->len);
        assert(dest->capacity == src->len);
        /* From Doxygen: "Allocator of the dest will be identical with parameter allocator." */
        assert(dest->allocator == allocator);
        /* From Doxygen: "A new memory zone is allocated for dest->buffer." */
        if (src->len > 0) {
            assert(dest->buffer != NULL);
            assert(dest->buffer != src->buffer);
            /* Contents should match */
            assert_bytes_match(dest->buffer, src->buffer, src->len);
        } else {
            /* From Doxygen: "If src buffer is null the dest will have a null buffer with a len and a capacity of 0" */
            assert(dest->buffer == NULL);
            assert(dest->len == 0);
            assert(dest->capacity == 0);
        }
    } else {
        /* From Doxygen: "Returns AWS_OP_ERR when memory can't be allocated." */
        assert(result == AWS_OP_ERR);
        /* On failure, dest should be zeroed out */
        assert(dest->buffer == NULL);
        assert(dest->len == 0);
        assert(dest->capacity == 0);
        assert(dest->allocator == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* src must remain unchanged */
    assert(src->len == old_src.len);
    assert(src->capacity == old_src.capacity);
    assert(src->buffer == old_src.buffer);
    assert(src->allocator == old_src.allocator);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(dest));
    assert(aws_byte_buf_is_valid(src));

    /* Cleanup */
    free(dest);
    free(src);
}
