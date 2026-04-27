#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and set up the source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old src state to check immutability */
    struct aws_byte_buf old_src = src;

    /* 2. Declare dest (uninitialized, will be written by function) */
    struct aws_byte_buf dest;

    /* 3. Set up allocator */
    struct aws_allocator *allocator = can_fail_allocator();
    __CPROVER_assume(allocator != NULL);

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator is set to the provided allocator */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* If src has no buffer, dest should be zeroed with just allocator set */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest capacity and len match src */
            assert(dest.capacity == src.capacity);
            assert(dest.len == src.len);
            /* dest has a newly allocated buffer (not NULL since capacity > 0) */
            assert(dest.buffer != NULL);
            /* Contents match */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }
    } else {
        /* On failure, dest should be zeroed out */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 6. src must not have been modified */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* 7. src validity invariant still holds */
    assert(aws_byte_buf_is_valid(&src));
}
