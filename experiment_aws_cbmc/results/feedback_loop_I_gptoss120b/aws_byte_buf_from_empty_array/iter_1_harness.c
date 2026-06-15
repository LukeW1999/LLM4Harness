#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* CBMC harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save a copy of src for immutability checks */
    struct aws_byte_buf old_src = src;

    /* 2. Declare destination buffer (uninitialized) and save its old state */
    struct aws_byte_buf dest;
    struct aws_byte_buf old_dest = dest; /* old_dest may contain nondet values */

    /* 3. Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 4. Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* allocator must be set to the one passed in */
        assert(dest.allocator == alloc);

        if (src.buffer == NULL) {
            /* When src has no buffer, dest should be a zeroed buffer */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* Otherwise dest must have a newly allocated buffer */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
        }
    } else {
        /* On failure the destination buffer is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 6. src must remain unchanged regardless of the result */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* 7. Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
