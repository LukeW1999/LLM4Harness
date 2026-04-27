#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

/**
 * Harness for aws_byte_buf_init_copy
 *
 * Analysis:
 * 1. Changed fields on success:
 *    - dest->buffer: newly allocated buffer (copy of src->buffer)
 *    - dest->len: set to src->len
 *    - dest->capacity: set to src->capacity
 *    - dest->allocator: set to the allocator parameter
 *
 * 2. Special case: if src->buffer is NULL:
 *    - dest is zeroed out (len=0, buffer=NULL, capacity=0)
 *    - dest->allocator is set to allocator
 *
 * 3. On failure (allocation fails):
 *    - dest is zeroed out (AWS_ZERO_STRUCT)
 *    - returns AWS_OP_ERR
 *
 * 4. Validity invariants:
 *    - src must remain valid (unchanged)
 *    - dest must be valid after the call
 */

void aws_byte_buf_init_copy_harness(void) {
    /* Set up src */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save src state to verify it's unchanged after the call */
    struct aws_byte_buf src_old = src;
    struct store_byte_from_buffer src_byte_storage;
    if (src.buffer && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_byte_storage);
    }

    /* Set up allocator */
    struct aws_allocator *allocator = can_fail_allocator();

    /* Set up dest (uninitialized) */
    struct aws_byte_buf dest;

    /* Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest->allocator must be set to the allocator parameter */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* Special case: src->buffer is NULL, dest is zeroed except allocator */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* Normal case: dest is a copy of src */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.buffer != NULL);
            /* The contents should match */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }

        /* dest must be valid */
        assert(aws_byte_buf_is_valid(&dest));

    } else {
        /* On failure: dest is zeroed out */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* dest must still be valid (zeroed struct is valid) */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* src must be unchanged */
    assert(src.buffer == src_old.buffer);
    assert(src.len == src_old.len);
    assert(src.capacity == src_old.capacity);
    assert(src.allocator == src_old.allocator);

    /* src must still be valid */
    assert(aws_byte_buf_is_valid(&src));

    /* Verify a byte from src is unchanged if it had data */
    if (src.buffer && src.len > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_byte_storage);
    }
}
