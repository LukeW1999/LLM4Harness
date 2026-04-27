// === STEP 1: SUCCESS PATH ===
// When aws_byte_cursor_from_c_str returns a valid aws_byte_cursor:
//   - cur.ptr: CHANGES to (uint8_t *)c_str
//   - cur.len: CHANGES to strlen(c_str) if c_str is not NULL, otherwise 0
//
// === STEP 2: FAILURE PATH ===
// There is no failure path in this function as it does not return an error code.
// The function always returns a struct aws_byte_cursor.
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   cur (struct aws_byte_cursor):
//     - ptr: CHANGED always
//     - len: CHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(&cur): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_from_c_str_harness() {
    const char *c_str = (const char *)any_ptr();
    struct aws_byte_cursor old_cur;

    // Initialize cur with non-deterministic values
    struct aws_byte_cursor cur;
    cur.ptr = (uint8_t *)any_ptr();
    cur.len = nondet_size_t();

    // Store the old state of cur
    old_cur = cur;

    // Call the function under test
    cur = aws_byte_cursor_from_c_str(c_str);

    // Assertions based on the analysis
    if (c_str != NULL) {
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == strlen(c_str));
    } else {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    }

    // Validity invariant
    assert(aws_byte_cursor_is_valid(&cur));
}
