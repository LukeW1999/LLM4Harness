#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old state of src for immutability checks */
    struct aws_byte_buf old_src = src;

    /* Destination buffer (output) */
    struct aws_byte_buf dest;

    /* Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 3. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Fields that must be set on success */
        assert(dest.allocator == alloc);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        if (src.capacity > 0) {
            assert(dest.buffer != NULL);
            /* Verify that the copied bytes match */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
        /* On failure the function zeroes the destination struct */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 4. src must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    /* 5. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
