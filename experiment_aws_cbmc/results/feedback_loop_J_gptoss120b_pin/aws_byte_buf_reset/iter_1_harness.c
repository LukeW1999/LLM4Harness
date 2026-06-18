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
    /* 1. Declare and bound data structures */
    struct aws_byte_buf src;
    struct aws_byte_buf dest;

    /* src must be bounded and have a concrete buffer (if capacity > 0) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* dest is uninitialized before the call – we just need space for the struct */
    /* (no pre‑condition on dest validity) */

    /* allocator to use */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Save old state */
    struct aws_byte_buf old_src = src;
    struct aws_byte_buf old_dest = dest; /* may be garbage, used for failure unchanged check */

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Post‑condition checks */

    /* validity invariants always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));

    /* src must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    if (result == AWS_OP_SUCCESS) {
        /* allocator of dest must be the one passed in */
        assert(dest.allocator == alloc);

        if (src.buffer == NULL) {
            /* when src has no buffer, dest should be a zeroed buf */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest must have a newly allocated buffer */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* contents must be a copy of src */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* on failure the dest struct is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        /* dest must be unchanged from its pre‑call garbage state only in the sense
           that the function guarantees it is zeroed, not that it preserves old values */
    }

    /* 5. No other fields to assert unchanged – all dest fields are specified above */
}
