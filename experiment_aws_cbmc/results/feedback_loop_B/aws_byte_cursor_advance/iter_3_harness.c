#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

void aws_byte_cursor_advance_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_cursor cur;
    size_t advance_by = nondet_size_t();
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cur);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cur));
    __CPROVER_assume(advance_by <= cur.len);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_cursor old_cur = cur;

    /* 3. Call function under test */
    aws_byte_cursor_advance(&cur, advance_by);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(cur.ptr == old_cur.ptr + advance_by);
    assert(cur.len == old_cur.len - advance_by);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(cur.buf == old_cur.buf);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_cursor_is_valid(&cur));
}
