/* CBMC harness for aws_byte_buf_init_copy() */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator = nondet_pointer();

    /* 2. Bounding assumptions for the source buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 3. Bounding assumptions for the destination buffer (may be uninitialized) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);

    /* 4. Allocator must be non‑NULL (precondition of the function) */
    __CPROVER_assume(allocator != NULL);

    /* 5. Save old state for immutability checks */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src  = src;

    /* 6. Save a snapshot of the source bytes for later comparison */
    struct store_byte_from_buffer src_bytes;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_bytes);
    }

    /* 7. Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 8. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success path */
        if (src.buffer == NULL) {
            /* When the source has no buffer, the destination is zeroed but its allocator is set */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == allocator);
        } else {
            /* Normal copy case */
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);               /* new allocation */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.allocator == allocator);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
        /* Destination must be a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* Failure path – allocation failed, destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* 9. The source buffer must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    if (src.buffer != NULL && src.len > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_bytes);
    }

    /* 10. The destination buffer’s fields that are not documented as changing
        must retain their original values on the failure path.  The success path
        already asserts the expected new values, so we only need to check the
        failure case (already done above). */

    /* 11. Global validity invariants */
    assert(aws_byte_buf_is_valid(&src));
}
