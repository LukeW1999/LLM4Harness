#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_cursor_eq_c_str_harness() {
    /* Step 1: create a valid aws_byte_cursor within bounds */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Step 2: create a valid null-terminated C string with no embedded nulls */
    size_t max_str_len = MAX_BUFFER_SIZE;
    size_t str_len;
    __CPROVER_assume(str_len <= max_str_len);
    char *c_str = malloc(str_len + 1);
    if (c_str) {
        /* Fill with non-deterministic bytes, ensuring no embedded nulls */
        for (size_t i = 0; i < str_len; i++) {
            uint8_t byte;
            c_str[i] = (char)byte;
            __CPROVER_assume(c_str[i] != '\0');
        }
        c_str[str_len] = '\0';  /* guarantee null termination */
    }

    /* Step 3: save old state for immutability checks */
    struct aws_byte_cursor old_cursor = cursor;
    char *old_c_str = NULL;
    if (c_str) {
        old_c_str = malloc(str_len + 1);
        if (old_c_str) {
            /* copy manually to avoid <string.h> */
            for (size_t i = 0; i <= str_len; i++) {
                old_c_str[i] = c_str[i];
            }
        }
    }

    /* Step 4: call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* Step 5: postcondition – cursor fields unchanged */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);

    /* Step 6: postcondition – c_str contents unchanged */
    if (c_str && old_c_str) {
        for (size_t i = 0; i <= str_len; i++) {
            assert(c_str[i] == old_c_str[i]);
        }
    }

    /* Step 7: postcondition – cursor remains valid */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* Step 8: check the return value against a direct byte‑by‑byte interpretation of equivalence */
    if (c_str) {
        bool expected = false;
        if (cursor.len == str_len) {
            expected = true;
            for (size_t i = 0; i < cursor.len; i++) {
                if (cursor.ptr[i] != (uint8_t)c_str[i]) {
                    expected = false;
                    break;
                }
            }
        }
        assert(result == expected);
    }

    /* clean up */
    free(c_str);
    free(old_c_str);
}
