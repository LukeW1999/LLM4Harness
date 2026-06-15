#include <assert.h>
#include <stddef.h>

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>

#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Non‑deterministic allocator (must be non‑NULL) */
    struct aws_allocator *alloc;
    __CPROVER_assume(alloc != NULL);

    /* 3. Destination buffer (will be overwritten) */
    struct aws_byte_buf dest;

    /* 4. Save old state of source (dest is allowed to change) */
    struct aws_byte_buf old_src = src;

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 6. Post‑conditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* fields that must be set on success */
        assert(dest.allocator == alloc);
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);

        if (src.capacity == 0) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            /* buffer contents must match source up to src.len */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }

        /* source must remain unchanged */
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.buffer == old_src.buffer);
        assert(src.allocator == old_src.allocator);
    } else {
        /* on failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* source must remain unchanged */
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.buffer == old_src.buffer);
        assert(src.allocator == old_src.allocator);
    }

    /* 7. Validity invariants must always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
