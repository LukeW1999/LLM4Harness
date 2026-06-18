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
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest; /* dest may be uninitialized */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Save old state BEFORE calling */
    struct aws_byte_buf old_src = src;
    struct aws_byte_buf old_dest = dest;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* allocator must be set to the provided allocator */
        assert(dest.allocator == alloc);

        /* length and capacity must match source's length and capacity */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        /* buffer must be non‑NULL iff source capacity > 0 */
        if (src.capacity == 0) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
        }

        /* contents of the new buffer must equal source's contents for src.len bytes */
        if (src.len > 0) {
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }

        /* source must remain unchanged */
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.buffer == old_src.buffer);
        assert(src.allocator == old_src.allocator);
    } else {
        /* On failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* destination must be a valid (empty) buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* source must remain unchanged */
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.buffer == old_src.buffer);
        assert(src.allocator == old_src.allocator);
    }

    /* 5. Assert fields that must NOT change regardless of result (dest fields not set above) */
    /* No additional dest fields to check beyond those already asserted in each branch */

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
