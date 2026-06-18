#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* nondet length for the string (bounded) */
    size_t max_len = 256;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_len);

    /* allocate memory for a possible C string */
    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);

    /* fill with nondet characters */
    for (size_t i = 0; i < len; ++i) {
        c_str[i] = nondet_char();
    }
    c_str[len] = '\0';

    /* allow the input to be NULL as well */
    char *input = nondet_bool() ? NULL : c_str;

    /* precondition: if not NULL, it must be a valid null‑terminated string */
    __CPROVER_assume(input == NULL || strlen(input) <= max_len);

    /* PRE‑CALL SNAPSHOT */
    char *input_orig = input;
    size_t input_len = 0;
    if (input) {
        while (input[input_len] != '\0') {
            input_len++;
        }
    }

    /* call the function under verification */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(input);

    /* ASSERT_POSTCONDITIONS_HERE */
}
