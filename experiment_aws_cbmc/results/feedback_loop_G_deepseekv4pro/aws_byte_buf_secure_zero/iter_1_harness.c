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
    struct aws_byte_buf dst;
    struct aws_byte_buf src;

    /* Bound src buffer size */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    /* Allocate src buffer member non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&src);
    /* Ensure src is a valid byte buffer */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Use aws_default_allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Save old state */
    struct aws_byte_buf old_src = src;

    /* 2. Call function under test */
    int result = aws_byte_buf_init_copy(&dst, allocator, &src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* From Doxygen: "Dest capacity and len will be equal to the src len." */
        assert(dst.capacity == old_src.len);
        assert(dst.len == old_src.len);
        /* "Allocator of the dest will be identical with parameter allocator." */
        assert(dst.allocator == allocator);

        /* If src buffer is null, dest buffer is also null */
        if (old_src.buffer == NULL) {
            assert(dst.buffer == NULL);
            assert(dst.len == 0);
            assert(dst.capacity == 0);
        } else {
            assert(dst.buffer != NULL);
            /* Verify that the contents were copied */
            if (old_src.len > 0) {
                assert_bytes_match(dst.buffer, old_src.buffer, old_src.len);
            }
            /* dest buffer is a new memory zone */
            assert(dst.buffer != old_src.buffer);
        }
    } else {
        /* On failure: memory allocation failure.
         * Doxygen: "AWS_OP_ERR when memory can't be allocated."
         * Implementation zeros the struct on failure. */
        assert(dst.allocator == allocator || dst.allocator == NULL);
        assert(dst.buffer == NULL);
        assert(dst.len == 0);
        assert(dst.capacity == 0);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* src must remain immutable */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    /* 6. Assert validity invariants always hold */
    assert(aws_byte_buf_is_valid(&dst));
    assert(aws_byte_buf_is_valid(&src));
}
