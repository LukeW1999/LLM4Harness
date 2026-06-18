#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_cursor src;

    /* Ensure src is valid and bounded */
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));

    /* Save old state of src */
    struct aws_byte_cursor old_src = src;
    struct store_byte_from_buffer old_src_bytes;
    save_byte_from_array(src.ptr, src.len, &old_src_bytes);

    int rv = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* Assert validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));

    /* src is unchanged */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);
    assert_byte_from_buffer_matches(src.ptr, &old_src_bytes);

    if (rv == AWS_OP_SUCCESS) {
        /* Success postconditions */
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
        /* Failure postconditions */
        assert(rv == AWS_OP_ERR);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }
}
