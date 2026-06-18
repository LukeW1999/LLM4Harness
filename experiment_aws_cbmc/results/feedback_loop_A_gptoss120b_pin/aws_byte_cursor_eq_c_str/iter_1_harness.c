#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/string.h>
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

    /* 2. Save old state */
    struct aws_byte_cursor old = cursor;

    /* 3. Create a nondeterministic null‑terminated C string */
    size_t c_len = nondet_size_t();
    __CPROVER_assume(c_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* make the string nondeterministic; only the terminator is required */
    c_str[c_len] = '\0';

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Assert unchanged fields */
    assert(cursor.ptr == old.ptr);
    assert(cursor.len == old.len);

    /* 6. Assert validity invariant */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* 7. Assert return value matches the underlying array comparison */
    bool expected = aws_array_eq_c_str(cursor.ptr, cursor.len, c_str);
    assert(result == expected);
}
