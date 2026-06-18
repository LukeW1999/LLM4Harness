#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Harness for aws_byte_buf_init_copy_from_cursor */
void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* 1. Declare inputs */
    struct aws_byte_buf dest;
    struct aws_allocator *alloc = aws_default_allocator();
    struct aws_byte_cursor src;

    /* nondet cursor length, bounded */
    src.len = nondet_size_t();
    __CPROVER_assume(src.len <= MAX_BUFFER_SIZE);

    /* allocate buffer for src if length > 0 */
    ensure_byte_cursor_has_allocated_buffer_member(&src);

    /* ensure src is a valid bounded cursor */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* 2. Save old state of src (dest is output, no need to save) */
    struct aws_byte_cursor old_src = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, alloc, src);

    /* 4. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* allocator must be set to the provided allocator */
        assert(dest.allocator == alloc);
        /* len and capacity must equal src.len */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        /* buffer pointer and contents */
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
        /* on failure the function zeroes dest */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 5. src must remain unchanged */
    assert(src.ptr == old_src.ptr);
    assert(src.len == old_src.len);

    /* 6. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));
}
