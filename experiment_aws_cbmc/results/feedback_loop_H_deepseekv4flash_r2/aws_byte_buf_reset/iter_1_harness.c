#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness() {
    /* Nondeterministic inputs */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Preconditions from function */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));
    __CPROVER_assume(allocator != NULL);
    /* dest is a pointer to a struct, no initial validity required (output only) */

    /* Optionally bound source size to limit state space */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));

    /* Ensure src->buffer is allocated if capacity > 0 */
    ensure_byte_buf_has_allocated_buffer_member(&src);

    /* Re-assert validity after allocation */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old src state for immutability check */
    struct aws_byte_buf old_src = src;

    /* Call the function */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));   /* src remains valid */

    if (result == AWS_OP_SUCCESS) {
        if (src.buffer == NULL) {
            /* src is empty with null buffer */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* src has a buffer */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* Content must match */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
        assert(dest.allocator == allocator);
    } else {
        /* Failure: dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);   /* or 0, depending on NULL representation */
    }

    /* src must not be modified */
    assert(src.allocator == old_src.allocator);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
}
