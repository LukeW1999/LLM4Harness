#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

/* CBMC harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc = nondet_uint8_t() ? (struct aws_allocator *)0x1 : (struct aws_allocator *)0x1;
    /* Ensure allocator is non‑NULL as required by the function precondition */
    __CPROVER_assume(alloc != NULL);

    /* Bound and initialise the source buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Bound the destination buffer (its internal pointer may be NULL) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* 2. Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src   = src;

    /* 3. Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 6. Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));

    /* 4. Post‑conditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* allocator of the destination must be the supplied allocator */
        assert(dest.allocator == alloc);

        /* length and capacity of the destination must equal the source length */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);

        /* buffer pointer and contents depend on whether the source had a buffer */
        if (src.buffer != NULL) {
            assert(dest.buffer != NULL);
            /* the newly allocated buffer must contain a copy of the source data */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
        /* On allocation failure the destination must be zero‑initialised */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* The source buffer must remain unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
    }

    /* 5. Fields that must not change regardless of the result (none for dest,
       all for src which is already asserted above) */
}
