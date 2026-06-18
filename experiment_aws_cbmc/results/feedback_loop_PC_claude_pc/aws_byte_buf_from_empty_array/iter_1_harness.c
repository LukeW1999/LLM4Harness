#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Set up dest (uninitialized output buffer) */
    struct aws_byte_buf dest;

    /* 2. Set up allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 3. Set up src (valid aws_byte_buf) */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 4. Save old src state for frame conditions */
    struct aws_byte_buf old_src = src;

    /* Save a byte from src buffer for content check */
    struct store_byte_from_buffer src_byte_storage;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_byte_storage);
    }

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 6. Assert postconditions */

    /* --- Success path --- */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest->allocator must equal the allocator parameter */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* If src buffer is null, dest should have null buffer, len=0, capacity=0 */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest capacity and len will be equal to src capacity and len */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);

            /* dest buffer must be non-null if src capacity > 0 */
            if (src.capacity > 0) {
                assert(dest.buffer != NULL);
            }

            /* dest buffer must be a copy of src buffer (first len bytes) */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }

            /* dest buffer must be different from src buffer (new allocation) */
            if (src.capacity > 0) {
                assert(dest.buffer != src.buffer);
            }
        }
    } else {
        /* --- Failure path --- */
        /* On failure, dest should be zeroed out */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* --- Frame conditions: src must not be modified --- */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* src content must be unchanged */
    if (old_src.buffer != NULL && old_src.len > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_byte_storage);
    }

    /* src must still be valid */
    assert(aws_byte_buf_is_valid(&src));
}
