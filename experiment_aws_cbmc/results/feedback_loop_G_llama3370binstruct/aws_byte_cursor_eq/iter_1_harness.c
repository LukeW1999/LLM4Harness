#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_eq_harness() {
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_cursor_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    bool result = aws_byte_cursor_eq(&a, &b);

    if (result) {
        assert(aws_array_eq(a.ptr, a.len, b.ptr, b.len));
    } else {
        assert(!aws_array_eq(a.ptr, a.len, b.ptr, b.len));
    }

    assert(a.ptr == old_a.ptr);
    assert(a.len == old_a.len);
    assert(b.ptr == old_b.ptr);
    assert(b.len == old_b.len);

    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
