#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

/* CBMC harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness() {
    /* 1. Allocate and bound source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Save old state of src for immutability checks */
    struct aws_byte_buf old_src = src;

    /* 3. Destination buffer (output) */
    struct aws_byte_buf dest;

    /* 4. Allocator (use default allocator as required) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* allocator field must be set to the passed allocator */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* When source has no buffer, dest should be zeroed (len/capacity 0, buffer NULL) */
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.buffer == NULL);
        } else {
            /* Successful allocation: dest mirrors src (except allocator) */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* Allocation failure: dest must be completely zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* 7. src must remain unchanged regardless of outcome */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);
    if (src.buffer != NULL) {
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    }

    /* 8. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
