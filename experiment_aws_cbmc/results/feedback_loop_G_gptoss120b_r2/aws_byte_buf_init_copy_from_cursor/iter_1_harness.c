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
    /* 1. Declare and bound data structures */
    struct aws_byte_buf dest;
    struct aws_byte_cursor src;

    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));

    /* src must have an allocated buffer member for the copy */
    ensure_byte_cursor_has_allocated_buffer_member(&src);

    /* Precondition from the implementation */
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* 2. Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_cursor old_src = src;

    /* 3. Call function under test */
    struct aws_allocator *allocator = aws_default_allocator();
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 4. Postconditions for success and failure */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be properly initialized */
        assert(dest.buffer != NULL);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);
        if (src.len > 0) {
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        }
    } else {
        /* allocation failed – dest should be an empty, valid buffer */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* src must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);

    /* 5. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));
}
