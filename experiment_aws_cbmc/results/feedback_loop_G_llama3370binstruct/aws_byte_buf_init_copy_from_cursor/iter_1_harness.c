#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old_dest = dest;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);
        assert(memcmp(dest.buffer, src.ptr, src.len) == 0);
    } else {
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(dest.allocator == allocator);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&dest));
}
