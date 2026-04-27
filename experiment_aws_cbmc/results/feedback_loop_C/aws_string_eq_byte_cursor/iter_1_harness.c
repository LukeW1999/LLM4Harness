// === STEP 1: SUCCESS PATH ===
// When aws_string_eq_byte_cursor returns true:
//   - str: UNCHANGED
//   - cur: UNCHANGED

// === STEP 2: FAILURE PATH ===
// When aws_string_eq_byte_cursor returns false:
//   - str: UNCHANGED
//   - cur: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   str (struct aws_string):
//     - allocator: UNCHANGED always
//     - len: UNCHANGED always
//     - bytes: UNCHANGED always
//   cur (struct aws_byte_cursor):
//     - ptr: UNCHANGED always
//     - len: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_string_is_valid(&str): YES (must hold after call)
//   - aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE): YES

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_cursor_harness() {
    struct aws_string *str = malloc(sizeof(*str));
    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_byte_cursor cur;
    ensure_byte_cursor_has_allocated_buffer_member(&cur);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));

    struct aws_string old_str = *str;
    struct aws_byte_cursor old_cur = cur;

    bool result = aws_string_eq_byte_cursor(str, &cur);

    // Frame conditions
    assert(str->allocator == old_str.allocator);
    assert(str->len == old_str.len);
    assert(str->bytes == old_str.bytes);
    assert(cur.ptr == old_cur.ptr);
    assert(cur.len == old_cur.len);

    // Validity invariants
    assert(aws_string_is_valid(str));
    assert(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
}
