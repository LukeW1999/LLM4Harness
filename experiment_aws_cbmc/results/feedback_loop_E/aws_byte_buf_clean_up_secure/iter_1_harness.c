#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_clean_up_secure_harness() {
    /* data structure */
    struct aws_byte_buf buf;

    /* allocate and bound buffer member */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* save old state */
    struct aws_byte_buf old = buf;

    /* call function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* postconditions */
    assert(aws_byte_buf_is_valid(&buf));

    /* check that buffer is zeroed */
    for (size_t i = 0; i < old.len; i++) {
        assert(buf.buffer[i] == 0);
    }

    /* unchanged fields */
    assert(buf.allocator == old.allocator);
    assert(buf.capacity == old.capacity);

    /* len should be reset to 0 */
    assert(buf.len == 0);

    /* buffer pointer should be NULL after cleanup */
    assert(buf.buffer == NULL);
}
