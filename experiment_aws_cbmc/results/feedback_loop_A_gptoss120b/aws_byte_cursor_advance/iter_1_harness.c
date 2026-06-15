#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare inputs and bound them */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc = nondet_pointer();

    /* allocator must be non‑NULL per precondition */
    __CPROVER_assume(alloc != NULL);

    /* bound the source buffer and allocate its internal array */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Save old state of source (dest is overwritten by the function) */
    struct aws_byte_buf old_src = src;

    /* 3. Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Post‑condition checks for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator field is set to the supplied allocator */
        assert(dest.allocator == alloc);

        if (src.buffer == NULL) {
            /* When source has no buffer, dest is zeroed */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* Allocation succeeded – dest mirrors src's size fields */
            assert(dest.buffer != NULL);
            assert(dest.capacity == src.capacity);
            assert(dest.len == src.len);

            /* The contents of the newly allocated buffer must match src */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }

        /* src must remain unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
    } else {
        /* Allocation failed – dest is zeroed */
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

    /* 5. Invariant: source buffer remains valid regardless of outcome */
    assert(aws_byte_buf_is_valid(&src));
}
