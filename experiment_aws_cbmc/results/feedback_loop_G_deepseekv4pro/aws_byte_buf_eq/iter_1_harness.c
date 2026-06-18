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
    struct aws_byte_buf src;

    __CPROVER_assume(dest != NULL);
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old_src = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(dest, allocator, &src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* From Doxygen: "Dest capacity and len will be equal to the src len." */
        assert(dest->len == src.len);
        assert(dest->capacity == src.len);

        /* From Doxygen: "If src buffer is null the dest will have a null buffer with a len and a capacity of 0" */
        if (src.buffer == NULL) {
            assert(dest->buffer == NULL);
            assert(dest->len == 0);
            assert(dest->capacity == 0);
        } else {
            /* From Doxygen: "Copies src buffer into dest" */
            assert(dest->buffer != NULL);
            if (src.len > 0) {
                assert_bytes_match(dest->buffer, src.buffer, src.len);
            }
        }

        /* From Doxygen: "Allocator of the dest will be identical with parameter allocator." */
        assert(dest->allocator == allocator);
    } else {
        /* On failure, dest should be zeroed out as per implementation */
        assert(dest->buffer == NULL);
        assert(dest->len == 0);
        assert(dest->capacity == 0);
        /* allocator is set to allocator parameter in the !src->buffer path,
           but in the allocation failure path it's zeroed */
    }

    /* 5. Assert fields that must NOT change on src (function does not modify src) */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));

    /* 6. Assert validity invariant always holds on dest */
    assert(aws_byte_buf_is_valid(dest));

    /* Cleanup */
    free(dest);
}
