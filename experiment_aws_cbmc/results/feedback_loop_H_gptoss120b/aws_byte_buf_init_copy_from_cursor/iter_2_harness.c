#include <assert.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* destination buffer (output) */
    struct aws_byte_buf dest = {0};

    /* allocator argument – nondeterministic pointer (may be NULL) */
    struct aws_allocator *allocator = nondet_pointer();

    /* source cursor */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save old state of source cursor for immutability checks */
    struct aws_byte_cursor old_src = src;

    /* Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* Post‑conditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest reflects a copy of src */
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
        /* allocation failure – dest remains zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* src must be unchanged */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);

    /* Invariant: both structures remain valid */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));
}
