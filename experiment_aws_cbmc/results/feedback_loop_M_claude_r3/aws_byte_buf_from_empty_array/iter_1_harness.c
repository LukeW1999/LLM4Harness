#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Set up source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Set up destination buffer (uninitialized, just needs to be a valid pointer) */
    struct aws_byte_buf dest;

    /* 3. Get allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 4. Save old src state to verify it is not modified */
    struct aws_byte_buf old_src = src;

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest is valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest->allocator == allocator parameter */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* If src buffer is null, dest has null buffer with len=0 and capacity=0 */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest capacity and len will be equal to src len */
            /* Actually from the implementation: dest = *src, then new buffer allocated */
            /* dest->len == src->len, dest->capacity == src->capacity */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.buffer != NULL);
            /* Contents match */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* On failure, dest should be zeroed out */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 7. src must not be modified */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* 8. src validity invariant still holds */
    assert(aws_byte_buf_is_valid(&src));
}
