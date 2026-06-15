#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_from_array_harness(void) {
    /* 1. Declare and bound input data */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    const void *bytes = src.ptr;
    size_t len = src.len;

    /* 2. Save old state of inputs (they are const, but we keep for completeness) */
    const void *old_bytes = bytes;
    size_t old_len = len;

    /* 3. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* 4. Post‑condition: fields of the returned cursor must match the inputs */
    assert(cur.ptr == (uint8_t *)old_bytes);
    assert(cur.len == old_len);

    /* 5. Unchanged inputs */
    assert(bytes == old_bytes);
    assert(len == old_len);

    /* 6. Validity invariant for the returned cursor */
    assert(aws_byte_cursor_is_valid(&cur));
}
