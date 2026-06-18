#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* allocator (default, never NULL) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* destination buffer (output) */
    struct aws_byte_buf dest;

    /* source cursor */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save old state of src for immutability checks */
    struct aws_byte_cursor old_src = src;

    /* Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* Global invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));

    /* src must remain unchanged */
    assert(src.ptr == old_src.ptr);
    assert(src.len == old_src.len);

    if (result == AWS_OP_SUCCESS) {
        /* On success, fields are set according to the specification */
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            for (size_t i = 0; i < src.len; ++i) {
                assert(dest.buffer[i] == src.ptr[i]);
            }
        } else {
            /* Zero‑length source yields a NULL buffer */
            assert(dest.buffer == NULL);
        }
    } else {
        /* On failure, dest must be zero‑initialized */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
