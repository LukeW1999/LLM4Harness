#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>

/* CBMC harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound the source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save a copy of the source state for immutability checks */
    struct aws_byte_buf old_src = src;

    /* 2. Destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* 3. Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 4. Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Destination must be a valid buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* Fields that are defined to change on success */
        assert(dest.allocator == alloc);
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);

        if (src.buffer == NULL) {
            /* When source has no buffer, destination must also have no buffer */
            assert(dest.buffer == NULL);
        } else {
            /* Destination must have a non‑NULL buffer and contain a copy of the source data */
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }

        /* Source must remain unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
    } else {
        /* On failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* Source must remain unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
    }

    /* 6. Validity invariants must always hold */
    assert(aws_byte_buf_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));
}
