#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_harness() {
    struct aws_byte_cursor a;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));

    struct aws_byte_cursor b;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    struct store_byte_from_buffer a_storage;
    struct store_byte_from_buffer b_storage;
    if (a.ptr != NULL && a.len > 0) {
        save_byte_from_array(a.ptr, a.len, &a_storage);
    }
    if (b.ptr != NULL && b.len > 0) {
        save_byte_from_array(b.ptr, b.len, &b_storage);
    }

    bool result = aws_byte_cursor_eq(&a, &b);

    bool expected = aws_array_eq(a.ptr, a.len, b.ptr, b.len);
    assert(result == expected);

    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);

    assert(b.len == old_b.len);
    assert(b.ptr == old_b.ptr);

    if (a.ptr != NULL && a.len > 0) {
        assert_byte_from_buffer_matches(a.ptr, &a_storage);
    }
    if (b.ptr != NULL && b.len > 0) {
        assert_byte_from_buffer_matches(b.ptr, &b_storage);
    }

    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
