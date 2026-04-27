#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <proof_helpers/utils.h>

/**
 * Harness for aws_byte_buf_init_copy
 *
 * From Doxygen:
 * - Copies src buffer into dest and sets the correct len and capacity.
 * - A new memory zone is allocated for dest->buffer.
 * - Dest capacity and len will be equal to the src len.
 * - Allocator of the dest will be identical with parameter allocator.
 * - If src buffer is null the dest will have a null buffer with a len and a capacity of 0.
 * - Returns AWS_OP_SUCCESS in case of success or AWS_OP_ERR when memory can't be allocated.
 */
void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and set up src */
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
        /* dest is valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* Allocator of dest is identical with parameter allocator */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* If src buffer is null, dest has null buffer with len and capacity of 0 */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* Dest capacity and len will be equal to the src len */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* A new memory zone is allocated for dest->buffer (different pointer) */
            assert(dest.buffer != NULL);
            /* Contents match */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }
    } else {
        /* On failure, dest should be zeroed out (AWS_ZERO_STRUCT) */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 6. Assert src is unchanged (immutability) */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));
}
