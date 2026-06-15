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
    /* 1. Declare and bound input structures */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest; /* output buffer, uninitialized */

    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Save old state of src (and dest for failure case) */
    struct aws_byte_buf old_src = src;
    struct aws_byte_buf old_dest = dest; /* may be nondet, used only for failure comparison */

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 4. Postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* allocator must be set to the provided allocator */
        assert(dest.allocator == allocator);

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
            /* Contents must be copied */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }

        /* src must remain unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
        if (src.buffer != NULL) {
            assert_bytes_match(src.buffer, old_src.buffer, src.len);
        }

    } else {
        /* On allocation failure, dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* src must remain unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
        if (src.buffer != NULL) {
            assert_bytes_match(src.buffer, old_src.buffer, src.len);
        }
    }

    /* 5. Invariant: both structures must be valid after the call */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
