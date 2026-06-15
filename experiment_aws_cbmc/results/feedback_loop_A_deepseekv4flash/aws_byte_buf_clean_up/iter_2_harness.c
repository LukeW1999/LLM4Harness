#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_clean_up_harness() {
    /* Non-deterministic byte buffer */
    struct aws_byte_buf buf;

    /* Ensure the buffer is either valid (with allocated buffer) or zeroed */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old allocator to check if it was non-null */
    struct aws_allocator *old_allocator = buf.allocator;

    /* Call the function */
    aws_byte_buf_clean_up(&buf);

    /* Postcondition: buffer is zeroed */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* Additional: if allocator was non-null, the buffer was freed (no double-free) */
    if (old_allocator != NULL) {
        /* The buffer pointer should have been freed; we cannot assert it's freed,
         * but we can assert it's NULL now. */
        assert(buf.buffer == NULL);
    }
}
