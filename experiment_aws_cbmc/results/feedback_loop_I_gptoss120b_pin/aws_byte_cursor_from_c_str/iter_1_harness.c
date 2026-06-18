#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Nondeterministic input string */
    const char *c_str;
    bool is_null = nondet_bool();

    if (is_null) {
        c_str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        /* allocate space for string plus terminating NUL */
        char *buf = malloc(len + 1);
        __CPROVER_assume(buf != NULL);
        /* fill with nondeterministic bytes */
        for (size_t i = 0; i < len; ++i) {
            buf[i] = (char)nondet_uint8_t();
        }
        buf[len] = '\0';
        c_str = buf;
    }

    /* 2. Save old state */
    const char *old_c_str = c_str;

    /* 3. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 4. Post‑condition checks */
    if (old_c_str == NULL) {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        assert(cur.ptr == (uint8_t *)old_c_str);
        assert(cur.len == strlen(old_c_str));
    }

    /* 5. Validity invariant */
    assert(aws_byte_cursor_is_valid(&cur));
}
