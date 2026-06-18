#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void aws_byte_buf_init_copy_harness() {
    /* Construct non-deterministic inputs */
    struct aws_byte_buf src;
    struct aws_byte_buf old_src;

    /* Bound and allocate src buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old state of src for immutability checks */
    old_src = src;

    /* Allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* Call the function */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest->allocator must be the passed allocator */
        assert(dest.allocator == allocator);

        /* dest->len equals src->len */
        assert(dest.len == src.len);

        /* dest->capacity equals src->capacity */
        assert(dest.capacity == src.capacity);

        if (src.buffer == NULL) {
            /* If src had no buffer, dest must be zeroed (except allocator) */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest must have a non‑null writable buffer */
            assert(dest.buffer != NULL);
            assert(AWS_MEM_IS_WRITABLE(dest.buffer, dest.capacity));
            /* Contents must match */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* On failure, dest must be fully zeroed */
        assert(dest.allocator == NULL);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        /* Zeroed dest is still valid */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* src must be unchanged */
    assert(aws_byte_buf_is_valid(&src));
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
}
