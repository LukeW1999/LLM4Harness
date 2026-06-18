#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* 1. Set up the source cursor */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save a snapshot of src before the call (frame condition) */
    struct aws_byte_cursor src_old = src;

    /* Save a byte from src for content verification */
    struct store_byte_from_buffer src_byte_storage;
    if (src.len > 0) {
        save_byte_from_array(src.ptr, src.len, &src_byte_storage);
    }

    /* 2. Set up the destination buffer (uninitialized, will be written by function) */
    struct aws_byte_buf dest;

    /* 3. Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 4. Call the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 5. Assert postconditions */

    /* --- Success path --- */
    if (result == AWS_OP_SUCCESS) {
        /* dest->len == src.len */
        assert(dest.len == src.len);
        /* dest->capacity == src.len */
        assert(dest.capacity == src.len);
        /* dest->allocator == allocator */
        assert(dest.allocator == allocator);
        /* If src.len == 0, buffer should be NULL */
        if (src.len == 0) {
            assert(dest.buffer == NULL);
        } else {
            /* buffer is non-NULL and contains a copy of src */
            assert(dest.buffer != NULL);
            /* Verify content: a byte from src matches the corresponding byte in dest */
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        }
        /* dest is a valid aws_byte_buf */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* --- Failure path --- */
        /* On failure, dest should have been zeroed (AWS_ZERO_STRUCT(*dest)) */
        /* The function zeros dest first, then returns error if allocation fails */
        /* dest.buffer == NULL (allocation failed) */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == allocator);
        /* src.len must have been > 0 for allocation to fail */
        assert(src.len > 0);
    }

    /* --- Frame conditions: src must not be modified --- */
    assert(src.len == src_old.len);
    assert(src.ptr == src_old.ptr);

    /* Verify src content is unchanged */
    if (src.len > 0) {
        assert_byte_from_buffer_matches(src.ptr, &src_byte_storage);
    }

    /* --- Validity invariants --- */
    assert(aws_byte_cursor_is_valid(&src));
}
