#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* 1. Declare data structures */
    struct aws_byte_buf dest;
    struct aws_byte_cursor src;
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Constrain src */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* 3. Save old state of src */
    struct aws_byte_cursor old_src = src;

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Fields that must be set on success */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);

        if (src.len > 0) {
            /* When length is non‑zero a buffer must be allocated */
            assert(dest.buffer != NULL);
            __CPROVER_assert(AWS_MEM_IS_WRITABLE(dest.buffer, src.len),
                             "dest.buffer must be writable for src.len bytes");
            /* Contents must match the source cursor */
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            /* Zero‑length copy may leave buffer NULL */
            assert(dest.buffer == NULL);
        }

        /* Validity invariant */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* On allocation failure the buffer must be NULL */
        assert(dest.buffer == NULL);
        /* No further guarantees about other fields */
    }

    /* 6. src must remain unchanged regardless of result */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);
    assert(aws_byte_cursor_is_valid(&src));
}
