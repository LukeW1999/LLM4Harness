#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_harness() {
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_cursor_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    bool result = aws_byte_cursor_eq(&a, &b);

    if (result) {
        assert(aws_array_eq(a.ptr, a.len, b.ptr, b.len));
    } else {
        assert(!aws_array_eq(a.ptr, a.len, b.ptr, b.len));
    }

    assert(a.len == a.len); /* unchanged */
    assert(a.ptr == a.ptr); /* unchanged */
    assert(b.len == b.len); /* unchanged */
    assert(b.ptr == b.ptr); /* unchanged */

    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
