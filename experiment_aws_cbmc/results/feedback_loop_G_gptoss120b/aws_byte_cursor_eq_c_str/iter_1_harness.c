#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "aws/common/byte_buf.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_eq_c_str_harness(void) {
    /* 1. Declare and bound the cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Allocate a nondeterministic null‑terminated C string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* The contents are nondeterministic; only the terminator matters */
    c_str[c_str_len] = '\0';

    /* 3. Save old state of the cursor */
    struct aws_byte_cursor old = cursor;

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Post‑condition: result must equal the underlying array comparison */
    bool expected = aws_array_eq_c_str(cursor.ptr, cursor.len, c_str);
    assert(result == expected);

    /* 6. Unchanged fields (cursor is const, should not be modified) */
    assert(cursor.ptr == old.ptr);
    assert(cursor.len == old.len);

    /* 7. Validity invariant must still hold */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* 8. Clean up */
    free(c_str);
}
