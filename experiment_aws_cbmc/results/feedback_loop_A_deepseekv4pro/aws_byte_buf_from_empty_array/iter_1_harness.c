#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Bound and validate the source buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Capture old state of src for immutability checks */
    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(src.buffer, src.len, &old_byte);

    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* Postconditions depending on success/failure */
    if (result == AWS_OP_SUCCESS) {
        /* dest capacity and len equal to src len */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        /* dest allocator is the passed allocator */
        assert(dest.allocator == alloc);
        /* If src had no buffer, dest must have no buffer */
        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
        } else {
            /* dest buffer allocated and copy of src data */
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
        /* dest is a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* Allocation failure zeroes the entire dest */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        /* Still a valid (empty) buffer */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* src must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(src.buffer == old_src.buffer);
    assert_byte_from_buffer_matches(src.buffer, &old_byte);

    /* src must remain valid */
    assert(aws_byte_buf_is_valid(&src));
}
