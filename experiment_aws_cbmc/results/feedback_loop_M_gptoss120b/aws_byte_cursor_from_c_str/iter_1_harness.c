#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Nondeterministic input string (may be NULL) */
    bool is_null = nondet_bool();
    char *c_str = NULL;
    size_t str_len = 0;

    /* Storage for checking immutability of the input buffer */
    struct store_byte_from_buffer saved_bytes = {0};

    if (!is_null) {
        /* 2. Bound the length of the string */
        str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* 3. Allocate memory for the string (including NUL terminator) */
        c_str = malloc(str_len + 1);
        __CPROVER_assume(c_str != NULL);

        /* 4. Fill the string with nondeterministic bytes */
        for (size_t i = 0; i < str_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[str_len] = '\0';

        /* 5. Save a snapshot of the allocated bytes for later immutability check */
        save_byte_from_array((const uint8_t *)c_str, str_len, &saved_bytes);
    }

    /* 6. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 7. Post‑condition: pointer field */
    assert(cur.ptr == (uint8_t *)c_str);

    /* 8. Post‑condition: length field */
    if (c_str != NULL) {
        assert(cur.len == str_len);
        /* 9. Immutability: the original bytes must remain unchanged */
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &saved_bytes);
    } else {
        assert(cur.len == 0);
    }

    /* 10. Validity invariant */
    assert(aws_byte_cursor_is_valid(&cur));
}
