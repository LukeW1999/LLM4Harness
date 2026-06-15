#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound the source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Declare the destination buffer (output) */
    struct aws_byte_buf dest;

    /* 3. Non‑deterministic, non‑NULL allocator */
    struct aws_allocator *alloc;
    __CPROVER_assume(alloc != NULL);

    /* 4. Save old state of the source (must stay unchanged) */
    struct aws_byte_buf old_src = src;

    /* 5. Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* allocator is stored in the destination */
        assert(dest.allocator == alloc);

        if (src.buffer == NULL) {
            /* source empty → destination empty */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* allocation succeeded → destination mirrors source */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* Allocation failed → destination zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 7. Source must be unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* 8. Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
