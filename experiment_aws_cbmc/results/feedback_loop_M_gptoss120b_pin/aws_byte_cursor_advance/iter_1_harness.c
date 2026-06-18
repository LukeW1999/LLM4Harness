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
    /* 1. Declare inputs and bound them */
    struct aws_byte_buf dest;
    struct aws_allocator *alloc = aws_default_allocator();

    struct aws_byte_cursor src;
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    __CPROVER_assume(src.len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save old state of src for immutability checks */
    struct aws_byte_cursor old_src = src;

    /* 2. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, alloc, src);

    /* 3. Global validity invariant */
    assert(aws_byte_buf_is_valid(&dest));

    /* 4. Postconditions for success */
    if (result == AWS_OP_SUCCESS) {
        /* Length and capacity equal to source length */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        /* Allocator must be the one passed in */
        assert(dest.allocator == alloc);
        /* Buffer pointer and contents */
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
        /* 5. Postconditions for failure */
        /* Allocation failed: dest should be zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 6. src must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);
    if (src.len > 0) {
        assert_bytes_match(src.ptr, old_src.ptr, src.len);
    }
    assert(aws_byte_cursor_is_valid(&src));
}
