#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_c_str_harness(void) {
    /* 1. Declare and bound the byte cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Declare and bound the C string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = (char *)malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* fill with nondet bytes and null‑terminate */
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = nondet_uint8_t();
    }
    c_str[c_str_len] = '\0';

    /* 3. Save old state */
    struct aws_byte_cursor old_cursor = cursor;
    char *old_c_str = c_str;

    /* 4. Call function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Post‑condition: result must equal the underlying array comparison */
    bool expected = aws_array_eq_c_str(cursor.ptr, cursor.len, c_str);
    assert(result == expected);

    /* 6. Unchanged fields on both success and failure paths */
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);
    assert(c_str == old_c_str);

    /* 7. Validity invariants */
    assert(aws_byte_cursor_is_valid(&cursor));
}
