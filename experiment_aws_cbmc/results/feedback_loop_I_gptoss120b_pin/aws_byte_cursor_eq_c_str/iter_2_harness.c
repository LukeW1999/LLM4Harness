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
    /* 1. Declare and bound the cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Create a nondeterministic null‑terminated C string without interior NULs */
    size_t c_len = nondet_size_t();
    __CPROVER_assume(c_len < MAX_BUFFER_SIZE);
    char *c_str = malloc(c_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_len; ++i) {
        c_str[i] = nondet_uint8_t();
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[c_len] = '\0';

    /* 3. Save old state of the cursor */
    struct aws_byte_cursor old = cursor;

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Unchanged fields */
    assert(cursor.ptr == old.ptr);
    assert(cursor.len == old.len);

    /* 6. Validity invariant */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* 7. Post‑conditions on success */
    if (result) {
        assert(cursor.len == c_len);
        assert_bytes_match(cursor.ptr, (const uint8_t *)c_str, c_len);
    }
}
