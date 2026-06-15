#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare inputs */
    struct aws_byte_buf dest;               /* output buffer */
    struct aws_byte_buf src;                /* source buffer */
    struct aws_allocator *alloc;            /* allocator */

    /* allocator must be non‑NULL per precondition */
    __CPROVER_assume(alloc != NULL);

    /* bound and allocate the source buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Save old state for immutability checks */
    struct aws_byte_buf old_src = src;
    struct aws_allocator *old_alloc = alloc;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Post‑condition checks for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* dest must reflect src */
        assert(dest.allocator == alloc);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);

        if (src.len == 0) {
            /* zero‑length source yields a NULL buffer */
            assert(dest.buffer == NULL);
        } else {
            /* non‑empty source yields a newly allocated buffer */
            assert(dest.buffer != NULL);
            /* contents must be identical */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* on error the destination is zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* 5. Fields that must remain unchanged regardless of result */
    /* allocator pointer itself is not modified */
    assert(alloc == old_alloc);

    /* source buffer must be unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);
    if (src.buffer != NULL && src.len > 0) {
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    }

    /* 6. Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
