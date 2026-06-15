#include <aws/common/byte_buf.h>
#include <assert.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare inputs */
    struct aws_byte_buf src;
    struct aws_byte_buf dest;
    struct aws_allocator *alloc = nondet_ptr();

    /* allocator must be non‑NULL (precondition of the function) */
    __CPROVER_assume(alloc != NULL);

    /* 2. Bound and initialise the source buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 3. Bound the destination buffer (its current contents are irrelevant) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* 4. Save old state for immutability checks */
    struct aws_byte_buf old_src  = src;
    struct aws_byte_buf old_dest = dest;

    /* 5. Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 6. src must remain unchanged */
    assert(src.buffer   == old_src.buffer);
    assert(src.len      == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* 7. allocator field of dest is always set to the supplied allocator */
    assert(dest.allocator == alloc);

    /* 8. Post‑condition checks for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid buffer */
        assert(aws_byte_buf_is_valid(&dest));

        if (old_src.buffer == NULL) {
            /* Empty source case – dest should be an empty buffer */
            assert(dest.buffer   == NULL);
            assert(dest.len      == 0);
            assert(dest.capacity == 0);
        } else {
            /* Allocation succeeded – dest mirrors src */
            assert(dest.buffer != NULL);
            assert(dest.len      == old_src.len);
            assert(dest.capacity == old_src.capacity);
            /* The contents of dest must be a copy of src */
            assert_bytes_match(dest.buffer, old_src.buffer, dest.len);
        }
    } else {
        /* Failure can only happen when src.buffer is non‑NULL and allocation fails */
        assert(old_src.buffer != NULL);

        /* On failure the destination is zeroed (except allocator) and remains valid */
        assert(dest.buffer   == NULL);
        assert(dest.len      == 0);
        assert(dest.capacity == 0);
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* 9. The destination buffer must always satisfy its invariant */
    assert(aws_byte_buf_is_valid(&dest));
}
