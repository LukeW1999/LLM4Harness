#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Set up src buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save a byte from src buffer for immutability check */
    struct store_byte_from_buffer src_byte;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_byte);
    }

    /* Save old src state to verify it's not modified */
    struct aws_byte_buf old_src = src;

    /* 2. Set up dest (uninitialized output) */
    struct aws_byte_buf dest;

    /* 3. Get allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest is a valid aws_byte_buf */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest->allocator is set to the passed allocator */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* If src buffer is null, dest should be zeroed with allocator set */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest capacity and len match src */
            assert(dest.capacity == src.capacity);
            assert(dest.len == src.len);

            /* dest->buffer is a new allocation (not the same pointer as src) */
            assert(dest.buffer != NULL);

            /* Contents of dest->buffer match src->buffer for src->len bytes */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }
    } else {
        /* On failure, dest should be zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 6. src must not be modified by the function */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* Verify src is still valid */
    assert(aws_byte_buf_is_valid(&src));

    /* Verify a byte from src buffer is unchanged if it had data */
    if (old_src.buffer != NULL && old_src.len > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_byte);
    }
}
