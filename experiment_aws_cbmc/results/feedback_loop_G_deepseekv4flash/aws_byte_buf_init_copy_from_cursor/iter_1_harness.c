#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_from_cursor_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf dest;
    struct aws_allocator *allocator;
    struct aws_byte_cursor src;

    /* Bounding and allocation for src */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Valid allocator */
    __CPROVER_assume(allocator != NULL);

    /* Save old state (src is passed by value, so we save its members) */
    struct aws_byte_cursor old_src = src;

    /* Call the function */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: dest reflects the cursor */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
        /* Failure: dest remains zeroed (as initialized by AWS_ZERO_STRUCT) */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* Unchanged: src is passed by value, so its contents remain unchanged */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);

    /* Validity invariants */
    assert(aws_byte_cursor_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));
}
