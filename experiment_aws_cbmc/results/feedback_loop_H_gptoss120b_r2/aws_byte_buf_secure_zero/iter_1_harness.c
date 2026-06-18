#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>

/* Harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare structures and allocator */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Bound the structures */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src,  MAX_BUFFER_SIZE));

    /* 3. Allocate buffer members for src (if any) */
    ensure_byte_buf_has_allocated_buffer_member(&src);

    /* 4. Assume src is a valid byte buffer (precondition of the function) */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 5. Save old state for immutability checks */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src  = src;

    /* Save a byte from src for later content‑equality check */
    struct store_byte_from_buffer src_byte;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_byte);
    }

    /* 6. Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 7. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Fields that must change on success */
        assert(dest.allocator == alloc);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        if (src.buffer != NULL) {
            assert(dest.buffer != NULL);
            if (src.len > 0) {
                assert_bytes_match(src.buffer, dest.buffer, src.len);
            }
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
        /* On failure the function zeroes the destination */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 8. Fields that must remain unchanged regardless of outcome */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    if (src.buffer != NULL && src.len > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_byte);
    }

    /* 9. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
