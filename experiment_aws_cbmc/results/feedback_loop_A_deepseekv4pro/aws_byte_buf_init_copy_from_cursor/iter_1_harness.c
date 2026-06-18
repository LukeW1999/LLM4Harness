#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    /* 1. Declare and bound the source cursor */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* The destination buf is uninitialised; the function will initialise it */
    struct aws_byte_buf dest;

    /* Allocator – use the default one which may fail */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Save old state of the source cursor and its backing bytes */
    size_t old_src_len = src.len;
    uint8_t *old_src_ptr = src.ptr;

    /* Because MAX_BUFFER_SIZE bounds the length, we can copy the whole buffer to a local array */
    uint8_t old_src_buf[MAX_BUFFER_SIZE];
    if (old_src_len > 0) {
        /* This is safe: we assumed src.ptr points to at least old_src_len readable bytes */
        memcpy(old_src_buf, old_src_ptr, old_src_len);
    }

    /* 3. Call the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 4. Assert postconditions for ALL return paths */

    /* ----- Validity invariants always hold ----- */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));

    /* Source cursor and its backing bytes must be completely unchanged */
    assert(src.len == old_src_len);
    assert(src.ptr == old_src_ptr);
    if (old_src_len > 0) {
        assert_bytes_match(old_src_buf, old_src_ptr, old_src_len);
    }

    if (result == AWS_OP_SUCCESS) {
        /* ---------- Success path ---------- */
        assert(dest.len == old_src_len);
        assert(dest.capacity == old_src_len);
        assert(dest.allocator == allocator);

        if (old_src_len == 0) {
            /* Null buffer expected */
            assert(dest.buffer == NULL);
        } else {
            /* Allocated buffer must be non‑null and contain a copy of the source */
            assert(dest.buffer != NULL);
            /* Compare the newly written bytes with the saved source bytes */
            assert_bytes_match(dest.buffer, old_src_buf, old_src_len);
            /* Sanity: the new buffer is distinct from the source pointer */
            assert(dest.buffer != old_src_ptr);
        }
    } else {
        /* ---------- Failure path (returned AWS_OP_ERR) ---------- */
        assert(result == AWS_OP_ERR);
        /* On failure the destination is kept zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
        /* Additionally, the failure can only happen when src.len > 0
           (allocation fails for a non‑empty source) */
        assert(old_src_len > 0);
    }
}
