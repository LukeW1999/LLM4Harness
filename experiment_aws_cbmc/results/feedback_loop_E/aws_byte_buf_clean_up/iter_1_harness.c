#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_clean_up_harness() {
    /* data structure */
    struct aws_byte_buf buf;

    /* parameters */
    struct aws_allocator *allocator = aws_default_allocator(); /* Precondition: allocator is non-null */

    /* Initialize buf with bounded and valid state */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    buf.allocator = allocator;

    /* Save old state for immutability checks */
    struct aws_byte_buf old = buf;

    /* Call function under test */
    aws_byte_buf_clean_up(&buf);

    /* Assert postconditions for BOTH success and failure paths */
    /* For aws_byte_buf_clean_up, there is no failure path, so we only need to handle success */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
}
