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
    struct aws_byte_cursor src;

    /* 2. Bound and allocate the input cursor */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    __CPROVER_assume(src.len <= MAX_BUFFER_SIZE);
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* 3. Choose allocator (default allocator is used throughout the library) */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 4. Save old state for immutability checks */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_cursor old_src = src;

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, alloc, src);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success: dest reflects src */
        assert(dest.allocator == alloc);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
        /* Failure (allocation error): dest is zeroed */
        assert(dest.allocator == NULL);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
    }

    /* 7. Fields that must remain unchanged regardless of result */
    /* dest is an output; only the fields above are allowed to change.
       The allocator pointer passed to the function is unchanged by definition. */
    (void)old_dest; /* suppress unused warning */

    /* src must be unchanged */
    assert(src.ptr == old_src.ptr);
    assert(src.len == old_src.len);
    if (src.len > 0) {
        assert_bytes_match(src.ptr, old_src.ptr, src.len);
    }

    /* 8. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));
}
