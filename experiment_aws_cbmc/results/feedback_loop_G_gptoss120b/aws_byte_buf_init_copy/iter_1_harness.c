#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf src;
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = (struct aws_allocator *)nondet_uint8_t(); /* nondet allocator pointer */

    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);

    /* nondet decide whether src has a backing buffer */
    if (nondet_bool()) {
        /* src has a buffer: ensure len is within capacity */
        __CPROVER_assume(src.len <= src.capacity);
    } else {
        /* src has no buffer: make it a zero‑length, zero‑capacity buffer */
        src.buffer = NULL;
        src.capacity = 0;
        src.len = 0;
    }

    /* src must be valid (precondition of the function) */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old state of src for immutability checks */
    struct aws_byte_buf old_src = src;

    /* 2. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 3. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        if (src.buffer == NULL) {
            /* When src has no buffer the function zeroes dest and only sets allocator */
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.buffer == NULL);
            assert(dest.allocator == allocator);
        } else {
            /* When src has a buffer the function copies fields and allocates a new buffer */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.allocator == allocator);
            assert(dest.buffer != NULL);
            assert(AWS_MEM_IS_WRITABLE(dest.buffer, dest.capacity));
        }
        /* src must remain unchanged */
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.buffer == old_src.buffer);
        assert(src.allocator == old_src.allocator);
    } else {
        /* Allocation failure: dest is zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
        /* src must remain unchanged */
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.buffer == old_src.buffer);
        assert(src.allocator == old_src.allocator);
    }

    /* 4. Validity invariants must hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
