#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

/* CBMC harness for aws_byte_buf_init_copy_from_cursor */
void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* 1. Declare inputs */
    struct aws_byte_buf dest;                     /* output buffer */
    struct aws_allocator *allocator = nondet_pointer(); /* allocator used for allocation */
    struct aws_byte_cursor src;                  /* source cursor */

    /* 2. Impose bounds and validity on the source cursor */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* 3. Ensure allocator is non‑NULL (aws_mem_acquire expects a valid allocator) */
    __CPROVER_assume(allocator != NULL);

    /* 4. Save old state of src (dest will be zeroed by the function) */
    struct aws_byte_cursor old_src = src;

    /* 5. Call the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest must reflect the source */
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);

        if (src.len == 0) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            /* the allocated buffer must contain a copy of the source bytes */
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        }

        /* src must be unchanged */
        assert(src.len == old_src.len);
        assert(src.ptr == old_src.ptr);
        if (src.len > 0) {
            assert_bytes_match(src.ptr, old_src.ptr, src.len);
        }
    } else {
        /* on failure the destination should remain zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);

        /* src must be unchanged */
        assert(src.len == old_src.len);
        assert(src.ptr == old_src.ptr);
        if (src.len > 0) {
            assert_bytes_match(src.ptr, old_src.ptr, src.len);
        }
    }

    /* 7. Invariants that must always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));
}
