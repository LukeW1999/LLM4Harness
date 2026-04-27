#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

void aws_byte_buf_clean_up_secure_harness() {
    /* data structure */
    struct aws_byte_buf buf;

    /* allocate and bound buffer member */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* save old state */
    struct aws_byte_buf old = buf;

    /* preconditions */
    __CPROVER_assume(aws_byte_buf_is_valid(&old));

    /* call function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* postconditions */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);
}
