#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

/* CBMC harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound the source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save a copy of src to check immutability */
    struct aws_byte_buf old_src = src;

    /* 2. Destination buffer (output) – no need to pre‑initialize, but we keep a copy for
       the failure case where the function zeroes it. */
    struct aws_byte_buf dest;
    /* Preserve the original (nondet) contents of dest for the failure path */
    struct aws_byte_buf old_dest = dest;

    /* 3. Use the default allocator (the only allocator allowed in harnesses) */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 4. Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 5. Post‑condition checks */

    /* src must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));

    if (result == AWS_OP_SUCCESS) {
        /* Success path */

        /* allocator is set to the one we passed */
        assert(dest.allocator == alloc);

        /* capacity and length are copied from src */
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);

        if (src.capacity != 0) {
            /* When src has a non‑zero capacity, dest must have a non‑NULL buffer */
            assert(dest.buffer != NULL);
            /* The first src.len bytes must be identical */
            assert_bytes_match(src.buffer, dest.buffer, src.len);
        } else {
            /* Zero‑capacity source results in a zeroed destination */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        }
    } else {
        /* Failure path – allocation could not be performed */
        /* The implementation zeroes the whole struct */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 6. In all cases the destination must be a valid byte buffer */
    assert(aws_byte_buf_is_valid(&dest));
}
