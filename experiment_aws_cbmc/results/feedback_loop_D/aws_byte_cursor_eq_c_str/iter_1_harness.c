// === STEP 1: SUCCESS PATH ===
// When aws_byte_cursor_eq_c_str returns true:
//   - cursor->ptr: UNCHANGED
//   - cursor->len: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_cursor_eq_c_str returns false:
//   - cursor->ptr: UNCHANGED
//   - cursor->len: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   cursor (struct aws_byte_cursor):
//     - ptr: UNCHANGED always
//     - len: UNCHANGED always
//   c_str (char*):
//     - N/A: c_str is a pointer to a constant character array, so it does not have fields
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(&cursor): YES (must hold after call)
//   - aws_byte_cursor_is_valid(&c_str): NO (c_str is not a struct, it's a pointer to a char array)
```

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_eq_c_str_harness() {
    struct aws_byte_cursor cursor;
    const char *c_str;
    struct aws_byte_cursor old_cursor = cursor;

    // Ensure the cursor has a valid allocated buffer member
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);

    // Ensure c_str points to a valid string literal
    c_str = "test_string";

    // Call the function under test
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    // === ASSERTIONS ===

    // Frame conditions
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);

    // Validity invariants
    assert(aws_byte_cursor_is_valid(&cursor));
}
