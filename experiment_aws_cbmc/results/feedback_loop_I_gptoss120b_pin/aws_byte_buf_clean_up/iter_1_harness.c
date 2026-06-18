#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* CBMC harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound input structures */
    struct aws_byte_buf src;
    struct aws_byte_buf dest;
    struct aws_allocator *alloc = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Save old state of src (dest is output, but we keep a copy for completeness) */
    struct aws_byte_buf old_src = src;
    struct aws_byte_buf old_dest = dest; /* dest is uninitialized before the call */

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));

    /* 5. src must remain unchanged regardless of the result */
    assert(src.allocator == old_src.allocator);
    assert(src.capacity == old_src.capacity);
    assert(src.len == old_src.len);
    assert(src.buffer == old_src.buffer);
    if (src.buffer != NULL && src.len > 0) {
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    }

    /* 6. Post‑conditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* allocator of dest must be the one passed in */
        assert(dest.allocator == alloc);

        if (src.buffer == NULL) {
            /* When src has no buffer, dest should be an empty buffer */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* When src has a buffer, dest must have a newly allocated buffer */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* The contents of dest must match src */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* On allocation failure the function zeroes dest */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
