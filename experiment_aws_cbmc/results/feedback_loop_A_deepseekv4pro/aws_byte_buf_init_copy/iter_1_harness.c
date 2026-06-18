#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf dest;
    struct aws_byte_buf src;

    /* Initialize and bound src */
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old state of src to check immutability */
    struct aws_byte_buf old_src = src;

    /* Allocator must be valid (non-NULL) */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* --- Postconditions for success --- */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest uses the provided allocator */
        assert(dest.allocator == allocator);

        if (src.buffer != NULL) {
            /* Source had a backing buffer: dest gets a new copy */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* New backing array must be different from src's */
            assert(dest.buffer != src.buffer);
            /* Buffer must be non-NULL if capacity > 0 */
            if (dest.capacity > 0) {
                assert(dest.buffer != NULL);
            }
            /* Contents are a copy of the first dest.len bytes of src */
            if (dest.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, dest.len);
            }
        } else {
            /* Source buffer was NULL: dest is an empty buffer with the given allocator */
            assert(dest.len == 0);
            assert(dest.buffer == NULL);
            assert(dest.capacity == 0);
        }
    } else {
        /* --- Postconditions for failure --- */
        /* On allocation failure, the whole dest structure is zeroed */
        assert(dest.len == 0);
        assert(dest.buffer == NULL);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        /* dest is still a valid (empty) byte buffer */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* --- Immutability: src must not be modified --- */
    assert(old_src.len == src.len);
    assert(old_src.capacity == src.capacity);
    assert(old_src.buffer == src.buffer);
    assert(old_src.allocator == src.allocator);
}
