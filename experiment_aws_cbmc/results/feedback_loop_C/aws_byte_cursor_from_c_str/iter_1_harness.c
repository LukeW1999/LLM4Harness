// === STEP 1: SUCCESS PATH ===
// When aws_byte_cursor_from_c_str returns a valid aws_byte_cursor:
//   - cur.ptr: CHANGES to (uint8_t *)c_str
//   - cur.len: CHANGES to strlen(c_str) if c_str is not NULL, otherwise 0
//
// === STEP 2: FAILURE PATH ===
// There is no failure path for this function as it always returns a valid aws_byte_cursor.
// The function does not modify any parameters in case of "failure" conditions (e.g., NULL input).
//   - cur.ptr: UNCHANGED if c_str is NULL
//   - cur.len: UNCHANGED if c_str is NULL
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   cur (struct aws_byte_cursor):
//     - ptr: CHANGED if c_str is not NULL, UNCHANGED if c_str is NULL
//     - len: CHANGED if c_str is not NULL, UNCHANGED if c_str is NULL
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(&cur): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_byte_cursor_from_c_str_harness() {
    const char *c_str = nondet_bool() ? NULL : (const char *)any_memory();
    struct aws_byte_cursor old_cur = { .ptr = NULL, .len = 0 };
    struct aws_byte_cursor cur = old_cur;

    if (c_str != NULL) {
        __CPROVER_assume(strlen(c_str) <= MAX_BUFFER_SIZE); // Ensure c_str is bounded
    }

    cur = aws_byte_cursor_from_c_str(c_str);

    // Assertions for success path
    if (c_str != NULL) {
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == strlen(c_str));
    } else {
        assert(cur.ptr == old_cur.ptr);
        assert(cur.len == old_cur.len);
    }

    // Validity invariant
    assert(aws_byte_cursor_is_valid(&cur));
}
