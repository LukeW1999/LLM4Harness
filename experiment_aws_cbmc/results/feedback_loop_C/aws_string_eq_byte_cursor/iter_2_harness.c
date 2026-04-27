#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_cursor_harness() {
    struct aws_string *str = malloc(sizeof(*str));
    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_byte_cursor cur;
    ensure_byte_cursor_has_allocated_buffer_member(&cur);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));

    // Ensure the byte buffer is allocated and has the same length as the string
    __CPROVER_assume(cur.len == str->len);
    __CPROVER_assume(__CPROVER_w_ok(cur.ptr, cur.len));
    __CPROVER_assume(__CPROVER_r_ok(cur.ptr, cur.len));
    __CPROVER_assume(__CPROVER_w_ok(str->bytes, str->len));
    __CPROVER_assume(__CPROVER_r_ok(str->bytes, str->len));

    struct aws_string old_str = *str;
    struct aws_byte_cursor old_cur = cur;

    bool result = aws_string_eq_byte_cursor(str, &cur);

    // Frame conditions
    assert(str->allocator == old_str.allocator);
    assert(str->len == old_str.len);
    assert(cur.ptr == old_cur.ptr);
    assert(cur.len == old_cur.len);

    // Validity invariants
    assert(aws_string_is_valid(str));
    assert(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
}
