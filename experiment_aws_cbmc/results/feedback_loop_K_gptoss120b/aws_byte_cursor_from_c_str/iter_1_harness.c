#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Helper to compute strlen on a possibly nondet string */
static size_t compute_strlen(const char *s) {
    size_t i = 0;
    while (s[i] != '\0') {
        i++;
    }
    return i;
}

void aws_byte_cursor_from_c_str_harness(void) {
    /* --- nondet allocation for the input C string ----------------------------------- */
    size_t max_len;
    __CPROVER_assume(max_len <= 256);               /* bound the length for tractability */

    char *c_str = malloc(max_len + 1);
    __CPROVER_assume(c_str != NULL);               /* allocation must succeed for the non‑NULL case */

    /* fill the buffer with nondet bytes */
    for (size_t i = 0; i < max_len; ++i) {
        c_str[i] = nondet_char();
    }
    /* ensure null‑termination at the end of the buffer */
    c_str[max_len] = '\0';

    /* make a copy of the original buffer to check the frame condition later */
    char *c_str_copy = malloc(max_len + 1);
    __CPROVER_assume(c_str_copy != NULL);
    memcpy(c_str_copy, c_str, max_len + 1);

    /* optionally make the pointer NULL to model the NULL‑input case */
    bool make_null = nondet_bool();
    if (make_null) {
        c_str = NULL;
    }

    /* --- call the function under test ------------------------------------------------ */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* --- postcondition checks -------------------------------------------------------- */
    if (c_str == NULL) {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        assert(cur.ptr == (uint8_t *)c_str);
        size_t expected_len = compute_strlen(c_str);
        assert(cur.len == expected_len);
    }

    /* --- frame condition: input buffer must be unchanged ----------------------------- */
    if (c_str != NULL) {
        for (size_t i = 0; i < max_len + 1; ++i) {
            assert(c_str[i] == c_str_copy[i]);
        }
    }

    /* clean up */
    free(c_str_copy);
    if (c_str != NULL) {
        free(c_str);
    }

    return 0;
}
