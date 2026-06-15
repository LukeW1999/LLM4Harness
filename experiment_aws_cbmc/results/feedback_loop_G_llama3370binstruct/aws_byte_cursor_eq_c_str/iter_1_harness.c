#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_c_str_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    char *c_str = nondet_string();
    size_t c_str_len = strlen(c_str);
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_cursor old_cursor = cursor;

    /* 3. Call function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        assert(aws_array_eq_c_str(cursor.ptr, cursor.len, c_str));
    } else {
        assert(!aws_array_eq_c_str(cursor.ptr, cursor.len, c_str));
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_cursor_is_valid(&cursor));
}
