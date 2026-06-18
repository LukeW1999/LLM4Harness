#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    /* Data structure bounds */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save old state of src for later comparison */
    struct store_byte_from_buffer src_storage;
    if (src.len > 0) {
        save_byte_from_array(src.ptr, src.len, &src_storage);
    }

    /* Allocator (must be valid) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Destination buffer (initial contents are irrelevant; function will zero it) */
    struct aws_byte_buf dest;

    /* Call the function */
    int rval = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* Postconditions */
    /* Source cursor remains valid (function takes it by value, but we check) */
    assert(aws_byte_cursor_is_valid(&src));

    if (rval == AWS_OP_SUCCESS) {
        /* Success path */
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
        /* Failure path (only possible if src.len > 0 and allocation fails) */
        assert(src.len > 0); /* must have been non-empty to attempt allocation */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
}
