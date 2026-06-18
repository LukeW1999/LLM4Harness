#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_c_str_harness(void) {
    /* 1. Declare and bound the cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Allocate a nondeterministic C string (may be NULL) */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    char *c_str = NULL;
    if (c_str_len > 0) {
        c_str = malloc(c_str_len + 1);
        __CPROVER_assume(c_str != NULL);
        /* contents are nondeterministic; ensure null‑termination */
        c_str[c_str_len] = '\0';
    } else {
        /* length zero: either NULL or a pointer to an empty string */
        if (nondet_bool()) {
            c_str = NULL;
        } else {
            c_str = malloc(1);
            __CPROVER_assume(c_str != NULL);
            c_str[0] = '\0';
        }
    }

    /* 3. Save old state */
    struct aws_byte_cursor old_cursor = cursor;
    const char *old_c_str = c_str;

    /* 4. Call function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Assert that no fields of the cursor changed */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);

    /* 6. Assert that the C‑string pointer did not change */
    assert(c_str == old_c_str);

    /* 7. The return value must match the underlying array comparison */
    assert(result == aws_array_eq_c_str(cursor.ptr, cursor.len, c_str));

    /* 8. Validity invariants must still hold */
    assert(aws_byte_cursor_is_valid(&cursor));
}
