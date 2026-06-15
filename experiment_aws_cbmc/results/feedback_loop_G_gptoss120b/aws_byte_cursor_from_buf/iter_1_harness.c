#include <assert.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_from_buf_harness(void) {
    /* 1. Declare and bound the input buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state for immutability checks */
    struct aws_byte_buf old = buf;

    /* 3. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    /* 4. Post‑conditions for the returned cursor */
    assert(cur.ptr == buf.buffer);   /* cursor points to the same buffer */
    assert(cur.len == buf.len);      /* cursor length equals buffer length */

    /* 5. Fields of the input buffer must remain unchanged */
    assert(buf.buffer   == old.buffer);
    assert(buf.len      == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator== old.allocator);

    /* 6. Validity invariants must still hold */
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_cursor_is_valid(&cur));
}
