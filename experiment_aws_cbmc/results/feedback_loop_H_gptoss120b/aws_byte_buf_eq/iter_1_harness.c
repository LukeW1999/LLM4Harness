#include <assert.h>
#include <stdlib.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc = malloc(sizeof(struct aws_allocator));

    /* allocator must be non‑NULL (precondition of the function) */
    __CPROVER_assume(alloc != NULL);

    /* bound the source buffer and allocate its internal array if needed */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);

    /* enforce the validity predicate for src (len ≤ capacity, etc.) */
    if (src.capacity == 0) {
        src.buffer = NULL;
        src.len = 0;
    } else {
        __CPROVER_assume(src.len <= src.capacity);
    }
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Save old state for immutability checks */
    struct aws_byte_buf old_src = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Post‑condition checks for both success and failure paths */

    /* src must remain unchanged regardless of the result */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator of dest is the one passed in */
        assert(dest.allocator == alloc);

        /* length and capacity are copied from src */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        if (src.buffer == NULL) {
            /* when src has no buffer, dest must also have no buffer */
            assert(dest.buffer == NULL);
        } else {
            /* dest must have a newly allocated buffer */
            assert(dest.buffer != NULL);
            /* the new buffer must be different from src's buffer */
            assert(dest.buffer != src.buffer);
            /* the contents up to src.len must be identical */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* on failure the function zeroes the destination */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* dest must still satisfy the validity predicate */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* 5. The allocator pointer itself never changes */
    assert(alloc != NULL);
}
