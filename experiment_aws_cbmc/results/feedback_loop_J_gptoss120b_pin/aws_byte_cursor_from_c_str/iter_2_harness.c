#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Nondeterministically decide if the input string pointer is NULL */
    bool is_null = nondet_bool();

    /* 2. If not NULL, allocate a bounded, null‑terminated string */
    char *c_str = NULL;
    size_t str_len = 0;
    if (!is_null) {
        /* bound the length of the string (must be > 0 for the helpers) */
        str_len = nondet_size_t();
        __CPROVER_assume(str_len > 0);
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* allocate space for the string plus the terminating NUL */
        c_str = malloc(str_len + 1);
        __CPROVER_assume(c_str != NULL);

        /* fill the string with nondeterministic bytes */
        for (size_t i = 0; i < str_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        /* ensure NUL termination */
        c_str[str_len] = '\0';
    }

    /* 3. Save a snapshot of the original bytes (if any) for immutability check */
    struct store_byte_from_buffer old_bytes;
    if (c_str != NULL) {
        save_byte_from_array((const uint8_t *)c_str, str_len, &old_bytes);
    }

    /* 4. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 5. Assert post‑conditions */
    assert(cur.ptr == (uint8_t *)c_str);

    size_t expected_len = (c_str != NULL) ? strlen(c_str) : 0;
    assert(cur.len == expected_len);

    assert(aws_byte_cursor_is_valid(&cur));

    if (c_str != NULL) {
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_bytes);
    }

    /* 6. Clean up */
    free(c_str);
}
