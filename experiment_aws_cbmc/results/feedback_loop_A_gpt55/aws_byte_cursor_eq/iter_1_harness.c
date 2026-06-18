#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_harness() {
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));

    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    struct store_byte_from_buffer old_a_byte;
    struct store_byte_from_buffer old_b_byte;
    save_byte_from_array(a.ptr, a.len, &old_a_byte);
    save_byte_from_array(b.ptr, b.len, &old_b_byte);

    bool result = aws_byte_cursor_eq(&a, &b);

    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);
    assert(b.len == old_b.len);
    assert(b.ptr == old_b.ptr);

    assert_byte_from_buffer_matches(a.ptr, &old_a_byte);
    assert_byte_from_buffer_matches(b.ptr, &old_b_byte);

    assert(result == aws_array_eq(a.ptr, a.len, b.ptr, b.len));

    if (result) {
        assert(a.len == b.len);
        assert_bytes_match(a.ptr, b.ptr, a.len);
    } else {
        assert(!aws_array_eq(a.ptr, a.len, b.ptr, b.len));
    }

    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
