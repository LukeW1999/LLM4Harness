#include <aws/common/byte_buf.h>
#include <aws/common/byte_order.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_buf_harness(void) {
    /* Symbolic input: aws_byte_buf */
    struct aws_byte_buf buf;
    ensure_byte_buf_is_valid(&buf, aws_default_allocator());

    /* Precondition: the buffer must be valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Snapshot of input state */
    size_t original_len = buf.len;
    uint8_t *original_ptr = buf.buffer;

    /* Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    /* ASSERT_POSTCONDITIONS_HERE */
    /* The returned cursor should reference the same memory and length as the original buffer */
    assert(cur.ptr == original_ptr);
    assert(cur.len == original_len);

    /* The original buffer must remain unchanged */
    assert(buf.buffer == original_ptr);
    assert(buf.len == original_len);
}
