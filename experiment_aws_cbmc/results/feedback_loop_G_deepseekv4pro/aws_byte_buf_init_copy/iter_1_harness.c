#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness() {
    /* Declare and bound source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Use the default allocator; it is a valid aws_allocator object */
    struct aws_allocator *allocator = aws_default_allocator();

    /* dest is uninitialised, exactly as the caller would have it */
    struct aws_byte_buf dest;

    /* Save old state of src for immutability checks */
    struct aws_byte_buf old_src = src;
    /* Store the original buffer pointer explicitly to detect pointer equality later */
    uint8_t * const old_src_buffer = src.buffer;
    size_t const old_src_len = src.len;
    size_t const old_src_capacity = src.capacity;
    struct aws_allocator * const old_src_allocator = src.allocator;

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 1. Source must remain completely unchanged (const correctness) */
    assert(src.len == old_src_len);
    assert(src.capacity == old_src_capacity);
    assert(src.allocator == old_src_allocator);
    assert(src.buffer == old_src_buffer);

    /* 2. Postconditions depending on return value */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid aws_byte_buf after successful init */
        assert(aws_byte_buf_is_valid(&dest));

        /* Allocator is the one we passed in */
        assert(dest.allocator == allocator);

        if (old_src_buffer == NULL) {
            /* If source had no buffer, dest must be zero length/capacity and no buffer */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* Source had a buffer – dest gets a new copy */
            assert(dest.buffer != NULL);
            assert(dest.buffer != old_src_buffer); /* New backing array */
            assert(dest.capacity == old_src_capacity);
            assert(dest.len == old_src_len);

            /* Contents are a copy of src's buffer */
            if (dest.len > 0) {
                assert_bytes_match(dest.buffer, old_src_buffer, dest.len);
            }
        }
    } else {
        /* On failure (allocation failed) dest must be zeroed out */
        assert(result == AWS_OP_ERR);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        /* A zero-initialised aws_byte_buf must still be valid */
        assert(aws_byte_buf_is_valid(&dest));
    }
}
