#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* nondeterministic choice of NULL or a valid C string */
    bool is_null = nondet_bool();
    char *c_str = NULL;
    size_t str_len = 0;

    if (!is_null) {
        /* bound the length of the string */
        str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* allocate space for the string plus terminating NUL */
        c_str = malloc(str_len + 1);
        __CPROVER_assume(c_str != NULL);

        /* fill with nondeterministic bytes (no interior NUL) */
        for (size_t i = 0; i < str_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[str_len] = '\0';
    }

    /* call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* post‑conditions */
    if (c_str == NULL) {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == str_len);
    }

    /* validity invariant */
    assert(aws_byte_cursor_is_valid(&cur));
}
