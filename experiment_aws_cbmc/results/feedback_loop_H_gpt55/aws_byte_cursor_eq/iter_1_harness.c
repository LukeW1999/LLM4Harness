#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_cursor_eq_harness(void) {
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

    struct store_byte_from_buffer old_byte_a;
    struct store_byte_from_buffer old_byte_b;
    save_byte_from_array(a.ptr, a.len, &old_byte_a);
    save_byte_from_array(b.ptr, b.len, &old_byte_b);

    bool result = aws_byte_cursor_eq(&a, &b);

    if (result) {
        assert(aws_array_eq(old_a.ptr, old_a.len, old_b.ptr, old_b.len));
        assert(a.len == b.len);
        assert_bytes_match(a.ptr, b.ptr, a.len);
    } else {
        assert(!aws_array_eq(old_a.ptr, old_a.len, old_b.ptr, old_b.len));
    }

    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);
    assert(b.len == old_b.len);
    assert(b.ptr == old_b.ptr);

    assert_byte_from_buffer_matches(a.ptr, &old_byte_a);
    assert_byte_from_buffer_matches(b.ptr, &old_byte_b);

    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
