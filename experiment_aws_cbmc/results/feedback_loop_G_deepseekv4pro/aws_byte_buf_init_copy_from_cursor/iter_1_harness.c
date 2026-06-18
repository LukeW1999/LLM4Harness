#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    /* output structure */
    struct aws_byte_buf dest;

    /* allocator (non-null) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* source cursor */
    struct aws_byte_cursor src;
    /* set up length bounded and allocate backing store */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    src.len = len;
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    /* ensure validity of source */
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* save copy to check immutability of source */
    struct aws_byte_cursor old_src = src;

    /* call the function under verification */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest fields should reflect a successful copy */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            /* content must be an exact copy of src */
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            /* zero-length source yields a NULL buffer */
            assert(dest.buffer == NULL);
        }
    } else {
        /* on failure dest is left zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* dest must always be valid after return */
    assert(aws_byte_buf_is_valid(&dest));

    /* source cursor must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);
}
