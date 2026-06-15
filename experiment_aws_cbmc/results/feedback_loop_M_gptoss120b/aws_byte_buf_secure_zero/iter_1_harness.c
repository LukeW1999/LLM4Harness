#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

/* CBMC proof harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness() {
    /* 1. Declare and bound source byte buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Destination buffer (output) */
    struct aws_byte_buf dest;
    /* dest is uninitialized; function will initialize it */

    /* 3. Allocator */
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    /* 4. Save old state for immutability checks */
    struct aws_byte_buf old_src = src;
    struct aws_byte_buf old_dest = dest; /* may be garbage, used only for failure path where dest is zeroed */

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 6. Post‑condition: source must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));

    /* 7. Validity invariant for destination */
    assert(aws_byte_buf_is_valid(&dest));

    /* 8. Branch on source buffer presence */
    if (src.buffer == NULL) {
        /* When source has no buffer, function always succeeds and zeroes dest */
        assert(result == AWS_OP_SUCCESS);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == alloc);
    } else {
        /* Source has a non‑NULL buffer */
        if (result == AWS_OP_SUCCESS) {
            /* Successful allocation */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.buffer != NULL);
            assert(dest.allocator == alloc);
            /* The newly allocated buffer must contain a copy of the source data */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            /* Allocation failed: dest must be zeroed */
            assert(result == AWS_OP_ERR);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.buffer == NULL);
            assert(dest.allocator == NULL);
        }
    }
}
