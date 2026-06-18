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

    /* 2. Allocate a nondeterministic C string */
    size_t c_len = nondet_size_t();
    __CPROVER_assume(c_len < MAX_BUFFER_SIZE);
    char *c_str = malloc(c_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* make it a proper null‑terminated string */
    c_str[c_len] = '\0';

    /* 3. Save old state for immutability checks */
    struct aws_byte_cursor old_cursor = cursor;
    struct store_byte_from_buffer cursor_storage;
    if (cursor.len > 0) {
        save_byte_from_array(cursor.ptr, cursor.len, &cursor_storage);
    }
    struct store_byte_from_buffer cstr_storage;
    if (c_len > 0) {
        save_byte_from_array((uint8_t *)c_str, c_len, &cstr_storage);
    }

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Post‑condition: result must match the underlying helper */
    bool expected = aws_array_eq_c_str(cursor.ptr, cursor.len, c_str);
    assert(result == expected);

    /* 6. Unchanged fields of the cursor */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);

    /* 7. Memory pointed to by cursor and c_str must be unchanged */
    if (cursor.len > 0) {
        assert_byte_from_buffer_matches(cursor.ptr, &cursor_storage);
    }
    if (c_len > 0) {
        assert_byte_from_buffer_matches((uint8_t *)c_str, &cstr_storage);
    }

    /* 8. Validity invariants */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* 9. Clean up */
    free(c_str);
}
