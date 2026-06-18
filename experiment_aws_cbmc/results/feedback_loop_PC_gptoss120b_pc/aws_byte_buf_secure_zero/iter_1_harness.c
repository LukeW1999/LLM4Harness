#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>

/* Maximum size for bounding the source buffer (defined in the Makefile) */
#ifndef MAX_BUFFER_SIZE
#   define MAX_BUFFER_SIZE 256
#endif

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare inputs */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Non‑deterministically allocate and bound the source buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 3. Save old state of inputs for immutability checks */
    struct aws_byte_buf old_src = src;
    struct aws_byte_buf old_dest = dest;   /* dest is uninitialized, but saved for completeness */

    /* 4. Save a byte from the source buffer to prove it is unchanged */
    struct store_byte_from_buffer src_byte;
    if (src.buffer != NULL && src.capacity > 0) {
        save_byte_from_array(src.buffer, src.capacity, &src_byte);
    }

    /* 5. Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 6. Post‑conditions */

    /* The function must always leave the source buffer unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(src.buffer == old_src.buffer);
    if (src.buffer != NULL && src.capacity > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_byte);
    }

    /* The allocator argument is not modified */
    assert(alloc == aws_default_allocator());

    /* The destination buffer must be a valid aws_byte_buf after the call */
    assert(aws_byte_buf_is_valid(&dest));

    if (result == AWS_OP_SUCCESS) {
        /* Success path */

        /* Allocator of the destination must be the one supplied */
        assert(dest.allocator == alloc);

        if (src.buffer == NULL) {
            /* When the source has no buffer, destination is zeroed */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* Destination must have its own allocated buffer */
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);          /* distinct allocation */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* The contents of the destination must match the source for src.len bytes */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* Failure path (allocation failure) */

        /* Destination is zeroed on failure */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 7. The destination buffer must remain valid regardless of the outcome */
    assert(aws_byte_buf_is_valid(&dest));
}
