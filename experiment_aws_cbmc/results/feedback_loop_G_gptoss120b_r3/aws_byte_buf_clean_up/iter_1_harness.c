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
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old state for immutability checks */
    struct aws_byte_buf old_src = src;
    struct aws_byte_buf old_dest = dest; /* dest may be uninitialized, but saved for comparison */

    /* 2. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 3. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        if (src.buffer == NULL) {
            /* When source buffer is NULL, destination should be empty */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* Destination must have a newly allocated buffer */
            assert(dest.buffer != NULL);
            /* Length and capacity must match source */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* Allocator must be the one passed in */
            assert(dest.allocator == allocator);
            /* Contents must be identical up to src.len */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
        /* Destination must be a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* Allocation failure: destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        /* Destination must still be a valid byte buffer (empty) */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* 4. Fields that must NOT change regardless of result */
    /* Source buffer must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* Destination allocator pointer should be either the passed allocator (on success) or NULL (on failure) */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.allocator == allocator);
    } else {
        assert(dest.allocator == NULL);
    }

    /* 5. Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&src));
}
