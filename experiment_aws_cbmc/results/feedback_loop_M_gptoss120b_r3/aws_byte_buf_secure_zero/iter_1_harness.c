#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* destination buffer (output) */
    struct aws_byte_buf dest;

    /* source cursor */
    struct aws_byte_cursor src;

    /* bound the source cursor */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* save old state of source (dest will be overwritten) */
    struct aws_byte_cursor old_src = src;

    /* call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, alloc, src);

    /* invariants that must always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));

    if (result == AWS_OP_SUCCESS) {
        /* fields that must be set on success */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == alloc);

        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
        /* on allocation failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* source must remain unchanged regardless of outcome */
    assert(src.ptr == old_src.ptr);
    assert(src.len == old_src.len);
}
