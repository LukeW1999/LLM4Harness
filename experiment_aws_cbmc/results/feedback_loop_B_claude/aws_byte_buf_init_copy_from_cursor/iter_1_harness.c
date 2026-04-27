#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* 1. Set up the destination buffer (will be zeroed by function) */
    struct aws_byte_buf dest;

    /* 2. Set up the source cursor with bounded size */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* 3. Set up allocator */
    struct aws_allocator *allocator = can_fail_allocator();

    /* 4. Save a byte from src for immutability check */
    struct store_byte_from_buffer old_src_byte;
    save_byte_from_array(src.ptr, src.len, &old_src_byte);

    /* 5. Save original src values */
    struct aws_byte_cursor old_src = src;

    /* 6. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 7. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: dest is properly initialized */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);

        /* If src.len > 0, buffer must be non-NULL and contain a copy */
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            /* If src.len == 0, buffer is NULL */
            assert(dest.buffer == NULL);
        }

        /* dest must be valid */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* On failure: dest was zeroed (AWS_ZERO_STRUCT was called) */
        /* The function zeros dest first, then fails on allocation */
        /* After AWS_ZERO_STRUCT and failed allocation: */
        assert(dest.len == 0 || dest.len == src.len);
        /* Actually on failure path: AWS_ZERO_STRUCT sets everything to 0,
           then buffer allocation fails, so dest remains zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 8. Source cursor must not have been modified */
    assert(src.ptr == old_src.ptr);
    assert(src.len == old_src.len);
    assert(aws_byte_cursor_is_valid(&src));

    /* 9. Source data must not have been modified */
    if (src.len > 0) {
        assert_byte_from_buffer_matches(src.ptr, &old_src_byte);
    }
}
