#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_copy_harness() {
    /* 1. Declare and bound the source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Destination buffer (output) – uninitialized */
    struct aws_byte_buf dest;

    /* 3. Allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 4. Save old state of src (must remain unchanged) */
    struct aws_byte_buf old_src = src;

    /* 5. Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 6. Post‑conditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* allocator of dest must be the one passed in */
        assert(dest.allocator == alloc);

        /* capacity and length must match the source */
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);

        if (src.buffer != NULL) {
            /* a new buffer must have been allocated */
            assert(dest.buffer != NULL);
            /* contents must be copied */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            /* source had no buffer → dest must be empty */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        }
    } else {
        /* On allocation failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 7. src must be unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    if (src.buffer != NULL) {
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    }

    /* 8. Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));
}
