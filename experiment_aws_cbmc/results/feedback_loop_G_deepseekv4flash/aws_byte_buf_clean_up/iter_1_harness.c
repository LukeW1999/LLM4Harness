#include "aws/common/byte_buf.h"
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 100
#endif

void aws_byte_buf_init_copy_from_cursor_harness() {
    /* Non-deterministic parameters */
    struct aws_allocator *allocator = nondet_bool() ? malloc(sizeof(struct aws_allocator)) : NULL;
    struct aws_byte_buf dest;
    struct aws_byte_cursor src;

    /* Bound and initialize src to be valid */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Preconditions */
    __CPROVER_assume(allocator != NULL);

    /* Save old state of src (value semantics, not modified) */
    struct aws_byte_cursor old_src = src;

    /* Call function */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
        assert(dest.allocator == NULL);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
    }

    /* Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));

    /* src is unchanged (by value) */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);
}
