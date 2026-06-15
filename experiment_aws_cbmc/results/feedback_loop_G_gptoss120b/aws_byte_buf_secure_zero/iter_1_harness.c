#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc;

    /* allocator must be non‑NULL as required by the function */
    __CPROVER_assume(alloc != NULL);

    /* bound the source buffer and allocate its internal array */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Save old state before the call */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src  = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Post‑condition checks for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* allocator of the destination must be the one passed in */
        assert(dest.allocator == alloc);

        /* capacity and length must match the source */
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);

        /* buffer handling */
        if (src.buffer == NULL) {
            /* when source buffer is NULL the destination buffer is also NULL */
            assert(dest.buffer == NULL);
        } else {
            /* otherwise a new buffer is allocated and its contents must equal the source */
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }

        /* source must be unchanged */
        assert(src.buffer   == old_src.buffer);
        assert(src.len      == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator== old_src.allocator);
    } else {
        /* on allocation failure the destination is zeroed */
        assert(dest.buffer   == NULL);
        assert(dest.len      == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator== NULL);

        /* source must be unchanged */
        assert(src.buffer   == old_src.buffer);
        assert(src.len      == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator== old_src.allocator);
    }

    /* 5. Validity invariants must always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
