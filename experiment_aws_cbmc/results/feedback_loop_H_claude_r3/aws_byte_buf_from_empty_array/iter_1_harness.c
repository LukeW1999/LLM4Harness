#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Set up dest buffer (output parameter, just needs to be a valid pointer) */
    struct aws_byte_buf dest;

    /* 2. Set up src buffer with bounded size */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 3. Save old src state to check immutability */
    struct aws_byte_buf old_src = src;

    /* 4. Get allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - dest is valid
         * - dest->allocator == allocator
         * - If src->buffer is NULL: dest has null buffer, len=0, capacity=0
         * - If src->buffer is non-NULL:
         *   - dest->capacity == src->capacity
         *   - dest->len == src->len
         *   - dest->buffer contains a copy of src->buffer[0..src->len)
         */
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            assert(dest.capacity == src.capacity);
            assert(dest.len == src.len);
            assert(dest.buffer != NULL);
            /* The contents should match for the first src.len bytes */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }
    } else {
        /* On failure: dest should be zeroed out (AWS_ZERO_STRUCT) */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 7. Assert src is unchanged (immutability) */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));
}
