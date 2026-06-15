#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_clean_up_secure_harness() {
    struct aws_byte_buf buf;

    /* Non-deterministically allocate and bound the buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save the original allocator pointer (the only field that should persist) */
    struct aws_allocator *old_allocator = buf.allocator;

    /* Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* Postcondition: len, capacity, and buffer are reset */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.buffer == NULL);

    /* The allocator pointer must remain unchanged */
    assert(buf.allocator == old_allocator);

    /* The resulting buffer is always valid */
    assert(aws_byte_buf_is_valid(&buf));
}
