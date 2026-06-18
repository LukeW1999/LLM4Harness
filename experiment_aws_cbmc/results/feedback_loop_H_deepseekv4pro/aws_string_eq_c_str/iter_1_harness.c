#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 4

void aws_string_eq_c_str_harness() {
    /* Determine whether the arguments will be NULL */
    bool str_is_null = nondet_bool();
    bool c_str_is_null = nondet_bool();

    struct aws_string *str = NULL;
    char *c_str = NULL;

    /* Allocate and initialise an aws_string if not NULL */
    if (!str_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);
        uint8_t *bytes = malloc(len);
        if (bytes) {
            for (size_t i = 0; i < len; i++) {
                bytes[i] = nondet_uint8_t();
            }
            struct aws_allocator *allocator = aws_default_allocator();
            str = aws_string_new_from_array(allocator, bytes, len);
            __CPROVER_assume(str != NULL);       /* allocation never fails in this harness */
        }
    }

    /* Allocate and initialise a C‑string if not NULL */
    if (!c_str_is_null) {
        size_t buf_size = MAX_STRING_LEN + 1;   /* one extra for the null terminator */
        char *buf = malloc(buf_size);
        if (buf) {
            for (size_t i = 0; i < buf_size; i++) {
                buf[i] = (char)nondet_uint8_t();
            }
            /* Place a null terminator inside the buffer */
            size_t null_pos = nondet_size_t();
            __CPROVER_assume(null_pos <= MAX_STRING_LEN);
            buf[null_pos] = '\0';
            c_str = buf;
        }
    }

    /* Compute the expected result independently */
    bool expected = false;
    if (str == NULL && c_str == NULL) {
        expected = true;
    } else if (str == NULL || c_str == NULL) {
        expected = false;
    } else {
        const uint8_t *str_bytes = aws_string_bytes(str);
        size_t str_len = str->len;

        /* Determine the length of the C‑string (excluding the null terminator) */
        size_t c_len = 0;
        while (c_str[c_len] != '\0') {
            if (c_len >= MAX_STRING_LEN) break;
            c_len++;
        }

        expected = (str_len == c_len);
        if (expected) {
            for (size_t i = 0; i < str_len; i++) {
                if (str_bytes[i] != (uint8_t)c_str[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }

    /* Save whether str was originally NULL */
    bool str_was_null = (str == NULL);

    /* Call the function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Check that the returned boolean matches the computed expectation */
    assert(result == expected);

    /* If str was a valid aws_string, it must still be valid (no mutation) */
    if (!str_was_null) {
        assert(aws_string_is_valid(str));
    }
}
