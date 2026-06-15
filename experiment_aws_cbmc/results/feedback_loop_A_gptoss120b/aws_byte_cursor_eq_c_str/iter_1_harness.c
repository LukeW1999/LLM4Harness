#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_byte_cursor_eq_c_str_harness(void) {
    /* 1. Declare and bound the aws_byte_cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Save old state */
    struct aws_byte_cursor old = cursor;

    /* 2. Allocate a nondeterministic null‑terminated C string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* make it a valid C string */
    c_str[c_str_len] = '\0';

    /* 3. Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 4. Post‑condition: function is pure – cursor must be unchanged */
    assert(cursor.ptr == old.ptr);
    assert(cursor.len == old.len);

    /* 5. The result must match the underlying array comparison */
    assert(result == aws_array_eq_c_str(old.ptr, old.len, c_str));

    /* 6. Validity invariant must still hold */
    assert(aws_byte_cursor_is_valid(&cursor));
}
