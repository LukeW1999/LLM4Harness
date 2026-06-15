#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_secure_zero_harness(void) {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;

    /* bound the buffer (MAX_BUFFER_SIZE defined in the proof environment) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* ensure the internal buffer member is allocated */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* pre‑condition: the buffer must be a valid aws_byte_buf */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state for later comparison */
    struct aws_byte_buf old_buf = buf;

    /* 3. Call the function under test */
    aws_byte_buf_secure_zero(&buf);

    /* 4. Post‑condition checks */
    /* the buffer pointer, capacity and allocator must remain unchanged */
    assert(buf.buffer    == old_buf.buffer);
    assert(buf.capacity  == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* length must be set to zero */
    assert(buf.len == 0);

    /* all bytes in the allocated region must be zeroed */
    if (buf.buffer != NULL) {
        for (size_t i = 0; i < buf.capacity; ++i) {
            __CPROVER_assert(buf.buffer[i] == 0, "byte is zero after secure zero");
        }
    }

    /* 5. Invariant: the resulting buffer must still be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
