#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* CBMC harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness(void) {
    /* -----------------------------------------------------------------
     * 1. Declare data structures and set up nondeterministic inputs
     * ----------------------------------------------------------------- */
    struct aws_byte_buf dest;                     /* output buffer */
    struct aws_byte_buf src;                      /* source buffer */
    struct aws_allocator *allocator = aws_default_allocator();

    /* bound the source buffer and allocate its internal array (if any) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* -----------------------------------------------------------------
     * 2. Save old state for immutability checks
     * ----------------------------------------------------------------- */
    struct aws_byte_buf old_src = src;   /* src must stay unchanged */
    struct aws_byte_buf old_dest = dest; /* dest is overwritten on success, zeroed on failure */

    /* -----------------------------------------------------------------
     * 3. Call the function under test
     * ----------------------------------------------------------------- */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* -----------------------------------------------------------------
     * 4. Post‑condition checks for both success and failure paths
     * ----------------------------------------------------------------- */
    if (result == AWS_OP_SUCCESS) {
        /* ----- fields that must be set on success ----- */
        assert(dest.allocator == allocator);
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);

        if (src.buffer == NULL) {
            /* source has no buffer → destination must also have no buffer */
            assert(dest.buffer == NULL);
        } else {
            /* destination must have a newly allocated buffer */
            assert(dest.buffer != NULL);
            /* the first src.len bytes must be identical */
            assert_bytes_match(src.buffer, dest.buffer, src.len);
        }

        /* ----- source must be unchanged ----- */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
    } else {
        /* ----- on allocation failure the destination is zeroed ----- */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* ----- source must be unchanged ----- */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
    }

    /* -----------------------------------------------------------------
     * 5. Invariants that must hold regardless of the outcome
     * ----------------------------------------------------------------- */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
