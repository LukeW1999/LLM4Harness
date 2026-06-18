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
    /* 1. Declare data structures */
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_cursor src;

    /* 2. Bound and allocate src cursor */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* 3. Save old state of src (dest is uninitialized before call) */
    struct aws_byte_cursor old_src = src;

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 5. Postconditions for success */
    if (result == AWS_OP_SUCCESS) {
        /* dest fields set according to src */
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }
        /* dest must be a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* On allocation failure dest remains zeroed */
        assert(dest.allocator == NULL);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        /* zeroed buffer is still considered valid */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* 6. src must be unchanged regardless of result */
    assert(src.ptr == old_src.ptr);
    assert(src.len == old_src.len);
    assert(aws_byte_cursor_is_valid(&src));
}
