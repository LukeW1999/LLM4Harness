#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_buf_clean_up_secure_harness(void) {
    struct aws_byte_buf buf;

    /* Use the default allocator */
    buf.allocator = aws_default_allocator();

    /* Bound the byte buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate the internal buffer using the allocator */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Assume the buffer is initially valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save the old state for later comparison */
    struct aws_byte_buf old = buf;

    /* Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* Post‑conditions */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.buffer == NULL);
    assert(buf.allocator == old.allocator);

    /* The buffer must still satisfy its validity invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
