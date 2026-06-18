#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* nondeterministically decide whether the input string is NULL */
    bool is_null = nondet_bool();
    char *c_str = NULL;
    size_t len = 0;

    if (!is_null) {
        /* nondeterministically choose a length bounded by MAX_BUFFER_SIZE */
        len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate space for the string plus the terminating NUL */
        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);

        /* fill the allocated buffer with nondeterministic bytes */
        for (size_t i = 0; i < len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        /* ensure NUL termination */
        c_str[len] = '\0';
    }

    /* call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* post‑conditions */
    assert(cur.ptr == (uint8_t *)c_str);
    if (c_str) {
        size_t expected_len = strlen(c_str);
        assert(cur.len == expected_len);
    } else {
        assert(cur.len == 0);
    }

    /* validity invariant must hold */
    assert(aws_byte_cursor_is_valid(&cur));
}
