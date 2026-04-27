// === STEP 1: SUCCESS PATH ===
// When aws_string_eq_byte_cursor returns true:
//   - str->bytes: UNCHANGED
//   - str->len: UNCHANGED
//   - cur->ptr: UNCHANGED
//   - cur->len: UNCHANGED

// === STEP 2: FAILURE PATH ===
// When aws_string_eq_byte_cursor returns false:
//   - str->bytes: UNCHANGED
//   - str->len: UNCHANGED
//   - cur->ptr: UNCHANGED
//   - cur->len: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// str (struct aws_string):
//   - allocator: UNCHANGED always
//   - len: UNCHANGED always
//   - bytes: UNCHANGED always
// cur (struct aws_byte_cursor):
//   - ptr: UNCHANGED always
//   - len: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_string_is_valid(&str): YES (must hold after call)
//   - aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE): YES

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_cursor_harness() {
    struct aws_string str;
    struct aws_byte_cursor cur;

    // Initialize str
    str.allocator = (struct aws_allocator *)nondet_ptr();
    str.len = nondet_size_t();
    str.bytes = (const uint8_t *)nondet_ptr();

    // Initialize cur
    cur.ptr = (uint8_t *)nondet_ptr();
    cur.len = nondet_size_t();

    // Save old state
    struct aws_string old_str = str;
    struct aws_byte_cursor old_cur = cur;

    // Ensure cur is bounded
    assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));

    // Call the function under test
    bool result = aws_string_eq_byte_cursor(&str, &cur);

    // Assertions for frame conditions and validity invariants
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);
    assert(str.bytes == old_str.bytes);
    assert(cur.ptr == old_cur.ptr);
    assert(cur.len == old_cur.len);
    assert(aws_string_is_valid(&str));
    assert(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
}
