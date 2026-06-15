#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_eq_byte_buf_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_cursor a;
    struct aws_byte_buf b;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));

    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* 2. Save old state BEFORE calling */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_buf old_b = b;

    /* 3. Call function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    /* 4. Assert postconditions */

    /* Unchanged fields for aws_byte_cursor */
    assert(a.ptr == old_a.ptr);
    assert(a.len == old_a.len);

    /* Unchanged fields for aws_byte_buf */
    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);

    /* Return value matches specification */
    assert(result == aws_array_eq(a.ptr, a.len, b.buffer, b.len));

    /* 5. Validity invariants must still hold */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
