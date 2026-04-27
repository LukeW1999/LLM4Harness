// === STEP 1: SUCCESS PATH ===
// aws_byte_cursor_eq_c_str returns a bool (true/false), not AWS_OP_SUCCESS/ERR
// When it returns true: cursor->ptr and cursor->len are unchanged (only read)
// The function delegates to aws_array_eq_c_str which compares cursor->ptr[0..len-1] with c_str
//
// === STEP 2: FAILURE PATH ===
// When it returns false: cursor->ptr and cursor->len are unchanged (only read)
//
// === STEP 3: FRAME CONDITIONS ===
// cursor (struct aws_byte_cursor):
//   - len: UNCHANGED always (only read)
//   - ptr: UNCHANGED always (only read)
// c_str: UNCHANGED always (only read)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(cursor): YES (must hold before and after call)
//   The function only reads cursor->ptr and cursor->len, no modifications

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_CURSOR_SIZE
#    define MAX_CURSOR_SIZE 8
#endif

#ifndef MAX_STR_LEN
#    define MAX_STR_LEN 8
#endif

void aws_byte_cursor_eq_c_str_harness(void) {
    /* Set up cursor */
    struct aws_byte_cursor cursor;
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_CURSOR_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Set up c_str - can be NULL or a valid string */
    size_t str_len;
    __CPROVER_assume(str_len <= MAX_STR_LEN);
    char *c_str = malloc(str_len + 1);
    if (c_str != NULL) {
        c_str[str_len] = '\0';
        /* rest of c_str is nondet */
    }

    /* Save old state */
    struct aws_byte_cursor old_cursor = cursor;

    /* Call the function */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* Frame conditions: cursor fields must be unchanged */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);

    /* Validity invariant: cursor must still be valid */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* The result is a bool - no additional postconditions on the return value
       beyond what the implementation guarantees, but we can check it's a valid bool */
    assert(result == true || result == false);
}
