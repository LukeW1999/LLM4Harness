#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Set up dest (uninitialized output buffer) */
    struct aws_byte_buf dest;

    /* 2. Set up allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 3. Set up src buffer */
    struct aws_byte_buf src;
    /* Bound the src buffer size to keep verification tractable */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, 10));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 4. Save old src state for frame conditions */
    struct aws_byte_buf old_src = src;

    /* Save a byte from src buffer for content immutability check */
    struct store_byte_from_buffer src_byte_storage;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_byte_storage);
    }

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 6. Assert postconditions */

    /* --- Return value paths --- */
    if (result == AWS_OP_SUCCESS) {
        /* On success, dest must be valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* Allocator of dest must be the passed allocator */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* If src buffer is null, dest has null buffer with len=0 and capacity=0 */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest capacity and len equal to src len */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);

            /* dest buffer is a new allocation (not the same pointer as src) */
            /* The content should match src for the first src.len bytes */
            if (src.len > 0) {
                assert(dest.buffer != NULL);
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }
    } else {
        /* On failure (AWS_OP_ERR), dest should be zeroed out */
        assert(result == AWS_OP_ERR);
        /* dest is zeroed: buffer=NULL, len=0, capacity=0, allocator=NULL */
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

    /* src content must not have changed */
    if (old_src.buffer != NULL && old_src.len > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_byte_storage);
    }

    /* src must still be valid */
    assert(aws_byte_buf_is_valid(&src));
}
