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

/* Harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound input structures */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest; /* output buffer, uninitialized */

    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Save old state of src (dest will be overwritten) */
    struct aws_byte_buf old_src = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator of dest must be the allocator passed in */
        assert(dest.allocator == alloc);

        /* len and capacity must match src's len and capacity */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        /* buffer pointer handling */
        if (src.capacity == 0) {
            /* when src has zero capacity, dest.buffer must be NULL */
            assert(dest.buffer == NULL);
        } else {
            /* when src has non‑zero capacity, allocation succeeded, so buffer must be non‑NULL */
            assert(dest.buffer != NULL);
        }

        /* src must remain unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
    } else {
        /* On error the function zeroes dest */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* dest must still satisfy the validity predicate */
        assert(aws_byte_buf_is_valid(&dest));

        /* src must remain unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
    }

    /* 5. Allocator pointer itself never changes */
    assert(alloc == aws_default_allocator());

    /* 6. Validity invariant for src holds regardless of outcome */
    assert(aws_byte_buf_is_valid(&src));
}
