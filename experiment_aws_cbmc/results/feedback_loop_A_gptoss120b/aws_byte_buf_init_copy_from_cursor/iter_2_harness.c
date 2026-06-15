#include <assert.h>
#include "aws/common/byte_buf.h"
#include "aws/common/allocator.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* 1. Declare data structures */
    struct aws_byte_buf dest;
    struct aws_byte_cursor src;
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Apply bounds and validity assumptions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));
    __CPROVER_assume(allocator != NULL);

    /* 3. Save old state for later comparison */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_cursor old_src = src;
    struct store_byte_from_buffer src_storage;
    if (src.len > 0) {
        save_byte_from_array(src.ptr, src.len, &src_storage);
    }

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 5. Post‑condition assertions using logical implication so both
       success and failure branches are exercised regardless of the
       actual result. */
    /* Success case */
    assert(!(result == AWS_OP_SUCCESS) ||
           (dest.len == src.len &&
            dest.capacity == src.len &&
            dest.allocator == allocator &&
            ((src.len == 0 && dest.buffer == NULL) ||
             (src.len > 0 && dest.buffer != NULL && assert_bytes_match(src.ptr, dest.buffer, src.len)))));

    /* Failure case (allocation failure) */
    assert(!(result != AWS_OP_SUCCESS) ||
           (dest.len == 0 &&
            dest.capacity == 0 &&
            dest.buffer == NULL &&
            dest.allocator == NULL));

    /* 6. Invariant: src must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);
    if (src.len > 0) {
        assert_byte_from_buffer_matches(src.ptr, &src_storage);
    }

    /* 7. Allocator pointer itself is not modified */
    assert(allocator != NULL);

    /* 8. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));
}
