#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* If not already defined, define a reasonable bound for buffer size */
#define MAX_BUFFER_SIZE 100

void aws_byte_buf_init_copy_harness() {
    /* Nondeterministic inputs */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;

    /* Preconditions: src must be a valid byte buffer */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));
    /* Bound the size of src to limit state space */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    /* Ensure src has allocated buffer if capacity > 0 */
    ensure_byte_buf_has_allocated_buffer_member(&src);
    /* Re-assert validity after allocation */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old src state for immutability check */
    struct aws_byte_buf old_src = src;

    /* Call the function (two-argument version) */
    int result = aws_byte_buf_init_copy(&dest, &src);

    /* Postconditions */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));

    if (result == AWS_OP_SUCCESS) {
        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == NULL);
        } else {
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
            assert(dest.allocator == src.allocator);
        }
    } else {
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* Ensure src is unchanged */
    assert(src.allocator == old_src.allocator);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
}
