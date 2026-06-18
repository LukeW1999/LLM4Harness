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

    /* 2. Snapshot old state of the cursor */
    struct aws_byte_cursor old_cursor = cursor;
    struct store_byte_from_buffer old_cursor_storage;
    if (cursor.ptr != NULL && cursor.len > 0) {
        save_byte_from_array(cursor.ptr, cursor.len, &old_cursor_storage);
    }

    /* 3. Allocate a nondeterministic C string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* make the string contents nondeterministic */
    __CPROVER_assume(__CPROVER_is_fresh(c_str, c_str_len + 1));
    c_str[c_str_len] = '\0'; /* null‑terminate */

    struct store_byte_from_buffer old_cstr_storage;
    if (c_str_len > 0) {
        save_byte_from_array((uint8_t *)c_str, c_str_len, &old_cstr_storage);
    }

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Post‑conditions: unchanged fields of the cursor */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);
    if (cursor.ptr != NULL && cursor.len > 0) {
        assert_byte_from_buffer_matches((uint8_t *)cursor.ptr, &old_cursor_storage);
    }

    /* 6. Post‑conditions: unchanged C‑string */
    if (c_str_len > 0) {
        assert_byte_from_buffer_matches((uint8_t *)c_str, &old_cstr_storage);
    }
    assert(c_str[c_str_len] == '\0');

    /* 7. Return value is a boolean – no additional constraints beyond being true/false */

    /* 8. Invariant: cursor remains valid */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* 9. Clean up */
    free(c_str);
}
