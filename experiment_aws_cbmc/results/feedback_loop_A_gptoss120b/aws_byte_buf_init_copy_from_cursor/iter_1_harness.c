#include <assert.h>
#include "aws/common/byte_buf.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf dest;
    struct aws_byte_cursor src;
    struct aws_allocator *allocator;

    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* allocator must be non‑NULL for the allocation call */
    __CPROVER_assume(allocator != NULL);

    /* 2. Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_cursor old_src = src;
    struct store_byte_from_buffer src_storage;
    if (src.len > 0) {
        save_byte_from_array(src.ptr, src.len, &src_storage);
    }

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 4. Assert postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* fields that must change on success */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);
        if (src.len == 0) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            assert_bytes_match(src.ptr, dest.buffer, src.len);
        }
    } else {
        /* on allocation failure the function zero‑initialises dest */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* src must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);
    if (src.len > 0) {
        assert_byte_from_buffer_matches(src.ptr, &src_storage);
    }

    /* allocator pointer itself is not modified */
    assert(allocator != NULL);

    /* 6. Assert validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));
}
