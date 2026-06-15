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

void aws_byte_buf_init_copy_harness(void) {
    /* allocator – always use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* destination buffer (output) */
    struct aws_byte_buf dest;
    /* source buffer (input) */
    struct aws_byte_buf src;

    /* bound the source buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));

    /* allocate the source buffer's internal array (may be overwritten later) */
    ensure_byte_buf_has_allocated_buffer_member(&src);

    /* nondeterministically set length within capacity */
    __CPROVER_assume(src.len <= src.capacity);

    /* nondeterministically decide whether the source buffer is “null” (no backing memory) */
    if (nondet_bool()) {
        src.buffer = NULL;
        src.capacity = 0;
        src.len = 0;
    }

    /* Save a copy of the source buffer for later immutability checks */
    struct aws_byte_buf old_src = src;

    /* If the source has a non‑null backing array, remember one byte for content comparison */
    struct store_byte_from_buffer src_byte;
    bool src_has_data = (src.buffer != NULL && src.len > 0);
    if (src_has_data) {
        save_byte_from_array(src.buffer, src.len, &src_byte);
    }

    /* Save the original state of the destination buffer (it is uninitialized before the call) */
    struct aws_byte_buf old_dest = dest;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* --------------------------------------------------------------------- */
    /* Post‑condition checks */
    /* --------------------------------------------------------------------- */

    if (result == AWS_OP_SUCCESS) {
        /* allocator must be set to the one passed in */
        assert(dest.allocator == allocator);

        /* length and capacity must match the source */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        if (src.buffer == NULL) {
            /* When source has no buffer, destination must also have no buffer */
            assert(dest.buffer == NULL);
        } else {
            /* Destination must have a newly allocated buffer */
            assert(dest.buffer != NULL);
            /* The first src.len bytes must be identical */
            assert_byte_from_buffer_matches(dest.buffer, &src_byte);
        }

        /* Destination must satisfy its validity invariant */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* On allocation failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* Destination must still be a valid (empty) buffer */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* --------------------------------------------------------------------- */
    /* Immutability of the source buffer (must be unchanged regardless of result) */
    /* --------------------------------------------------------------------- */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));

    /* --------------------------------------------------------------------- */
    /* Fields of the destination that are not specified to change are
       implicitly unchanged on the error path (already covered above). */
    /* --------------------------------------------------------------------- */
    if (result != AWS_OP_SUCCESS) {
        /* On error the destination should be exactly the zeroed state */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }
}
