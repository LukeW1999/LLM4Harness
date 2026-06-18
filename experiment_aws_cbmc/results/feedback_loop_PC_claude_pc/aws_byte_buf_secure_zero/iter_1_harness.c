#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Harness for aws_byte_buf_init_copy:
 *
 * Doxygen says:
 * - Copies src buffer into dest and sets the correct len and capacity.
 * - A new memory zone is allocated for dest->buffer.
 * - dest capacity and len will be equal to the src len.
 * - Allocator of the dest will be identical with parameter allocator.
 * - If src buffer is null the dest will have a null buffer with a len and a capacity of 0.
 * - Returns AWS_OP_SUCCESS in case of success or AWS_OP_ERR when memory can't be allocated.
 */
void aws_byte_buf_init_copy_harness(void) {
    /* 1. Set up src */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save src state before call to check it is not modified */
    struct aws_byte_buf src_old = src;
    /* Save a byte from src buffer for content check */
    struct store_byte_from_buffer src_byte_storage;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_byte_storage);
    }

    /* 2. Set up allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* 3. Declare dest (uninitialized, will be written by function) */
    struct aws_byte_buf dest;

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 5. Assert postconditions */

    /* --- Return value --- */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* --- Success path --- */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator of dest must equal the parameter allocator */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* If src buffer is null, dest has null buffer with len=0 and capacity=0 */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest capacity and len will be equal to the src len */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);

            /* dest->buffer is a new allocation (not the same pointer as src->buffer) */
            /* We can't assert pointer inequality in general, but we can assert it's non-null if src.len > 0 */
            if (src.capacity > 0) {
                assert(dest.buffer != NULL);
            }

            /* Content of dest->buffer matches src->buffer for src->len bytes */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }
    } else {
        /* --- Failure path --- */
        /* On failure, dest should be zeroed out (AWS_ZERO_STRUCT) */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* --- Frame: src must not be modified --- */
    assert(src.buffer == src_old.buffer);
    assert(src.len == src_old.len);
    assert(src.capacity == src_old.capacity);
    assert(src.allocator == src_old.allocator);

    /* Check that src buffer contents are unchanged */
    if (src.buffer != NULL && src.len > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_byte_storage);
    }

    /* --- Invariants --- */
    assert(aws_byte_buf_is_valid(&src));
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
    }
}
