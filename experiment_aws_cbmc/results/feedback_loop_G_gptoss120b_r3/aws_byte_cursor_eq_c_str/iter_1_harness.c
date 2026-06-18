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
    /* 1. Declare and bound the byte cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Declare a nondeterministic C string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    if (c_str) {
        /* nondeterministic contents, ensure null‑termination */
        __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));
        c_str[c_str_len] = '\0';
    }

    /* 3. Save old state of the cursor */
    struct aws_byte_cursor old = cursor;

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Post‑condition: fields of cursor must be unchanged */
    assert(cursor.len == old.len);
    assert(cursor.ptr == old.ptr);
    (void)result; /* result is nondeterministic, no further constraints */

    /* 6. Validity invariant must still hold */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* 7. Clean up */
    free(c_str);
}
