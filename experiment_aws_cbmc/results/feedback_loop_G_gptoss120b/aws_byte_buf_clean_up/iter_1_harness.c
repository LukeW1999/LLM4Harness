#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare structures */
    struct aws_byte_buf src;
    struct aws_byte_buf dest;
    struct aws_allocator *alloc;

    /* 2. Non‑deterministic allocator, must be non‑NULL */
    __CPROVER_assume(alloc != NULL);

    /* 3. Bound and allocate src buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 4. Save old state of src (must stay unchanged) */
    struct aws_byte_buf old_src = src;

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 6. Post‑conditions for success */
    if (result == AWS_OP_SUCCESS) {
        /* src must remain unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);

        if (src.buffer == NULL) {
            /* When source has no buffer, dest is a zero‑length buffer with the allocator set */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == alloc);
        } else {
            /* Destination must have a newly allocated buffer with copied contents */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.allocator == alloc);
            /* Verify that the copied bytes match the source */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* 7. Post‑conditions for failure */
        /* On allocation failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* src must remain unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
    }

    /* 8. Invariant: both structures must be valid after the call */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
