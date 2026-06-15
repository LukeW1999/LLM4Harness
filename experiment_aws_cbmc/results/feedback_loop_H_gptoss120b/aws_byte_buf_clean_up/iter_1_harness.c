#include <assert.h>
#include <stddef.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* allocator argument – nondeterministic but non‑NULL */
    struct aws_allocator *alloc;
    __CPROVER_assume(alloc != NULL);

    /* source buffer – must be a valid, bounded byte buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* destination buffer – bounded (its contents will be overwritten) */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* Save old state for immutability checks */
    struct aws_byte_buf old_src = src;
    struct aws_byte_buf old_dest = dest;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* Post‑conditions for the success path */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator field is set to the allocator argument */
        assert(dest.allocator == alloc);

        /* len and capacity must match the source */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        if (src.buffer == NULL) {
            /* When source has no buffer, dest must be empty */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* A new buffer must have been allocated */
            assert(dest.buffer != NULL);
            /* The new buffer must contain a copy of the source data */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
            /* The new buffer must be distinct from the source buffer */
            assert(dest.buffer != src.buffer);
        }
    } else {
        /* Failure path – allocation failed, dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* dest must still satisfy its validity invariant */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* The source buffer must be unchanged regardless of the outcome */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));

    /* In the success case, fields of dest that are not mentioned as changing
       remain as specified above; in the failure case they are all zero. */
    /* (All required assertions have been emitted above.) */
}
