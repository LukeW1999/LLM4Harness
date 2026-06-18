#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness(void) {
    /* Set up source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old src state to check immutability */
    struct aws_byte_buf old_src = src;

    /* Set up dest buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* Use default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Assert validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));

    /* Assert src is unchanged (immutability) */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - dest->allocator == allocator parameter
         * - If src->buffer is NULL: dest has null buffer, len=0, capacity=0
         * - If src->buffer is non-NULL:
         *   - dest->capacity == src->capacity
         *   - dest->len == src->len
         *   - dest->buffer is a new allocation with src->len bytes copied
         */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* src has null buffer */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* src has non-null buffer */
            assert(dest.capacity == src.capacity);
            assert(dest.len == src.len);
            assert(dest.buffer != NULL);
            /* Contents should match for the first src.len bytes */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }
    } else {
        /* On failure:
         * - dest is zeroed out (AWS_ZERO_STRUCT)
         */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
