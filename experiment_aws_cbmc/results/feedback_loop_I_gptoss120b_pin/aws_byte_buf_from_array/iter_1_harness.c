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
    /* 1. Declare and bound the source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* 3. Save old state for immutability checks */
    struct aws_byte_buf old_src = src;
    struct aws_byte_buf old_dest = dest; /* not required but kept for symmetry */

    /* 4. Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 5. Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 6. The destination must always be a valid byte buffer */
    assert(aws_byte_buf_is_valid(&dest));

    if (result == AWS_OP_SUCCESS) {
        /* ----- Success path postconditions ----- */
        /* allocator is set to the one passed in */
        assert(dest.allocator == alloc);

        if (src.buffer == NULL) {
            /* When source has no buffer, destination is zeroed */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* Destination must have a newly allocated buffer */
            assert(dest.buffer != NULL);
            /* Length and capacity must match the source */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* The contents must be an exact copy of the source */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* ----- Failure path postconditions ----- */
        /* On allocation failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* ----- Source buffer must remain unchanged ----- */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    if (src.buffer != NULL && src.len > 0) {
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    }

    /* ----- Destination buffer fields that are never modified ----- */
    /* (none besides those already asserted above) */
}
