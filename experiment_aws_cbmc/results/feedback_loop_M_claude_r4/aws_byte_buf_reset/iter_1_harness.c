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

    /* Set up destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* Use default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Save old src state to verify it is not modified */
    struct aws_byte_buf old_src = src;

    /* Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest allocator must be the passed allocator */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* If src buffer is null, dest should be zeroed with the allocator set */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest capacity and len will be equal to src len */
            assert(dest.capacity == src.capacity);
            assert(dest.len == src.len);
            assert(dest.buffer != NULL);
            /* Contents should match */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* On failure, dest should be zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* src must not be modified */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* src must still be valid */
    assert(aws_byte_buf_is_valid(&src));
}
