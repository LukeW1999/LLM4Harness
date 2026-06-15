#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Non‑deterministically decide whether the input pointer is NULL */
    bool is_null = nondet_bool();

    const char *c_str;
    size_t str_len = 0;               /* length of the string when c_str != NULL */

    if (!is_null) {
        /* 2. Choose a bounded length for the string (including space for the terminator) */
        size_t max_len = MAX_BUFFER_SIZE;               /* bound provided by the proof harness */
        __CPROVER_assume(max_len > 0);
        str_len = nondet_size_t();
        __CPROVER_assume(str_len <= max_len);

        /* 3. Allocate a buffer that can hold the string plus the terminating NUL */
        char *buf = malloc(str_len + 1);
        __CPROVER_assume(buf != NULL);

        /* 4. Fill the buffer with nondet bytes, but ensure no early NUL byte */
        for (size_t i = 0; i < str_len; ++i) {
            buf[i] = nondet_uint8_t();
            __CPROVER_assume(buf[i] != '\0');
        }
        /* 5. Place the terminating NUL */
        buf[str_len] = '\0';

        c_str = buf;
    } else {
        c_str = NULL;
        str_len = 0;
    }

    /* 6. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 7. Post‑condition checks */
    if (c_str == NULL) {
        /* When the input pointer is NULL the cursor must be empty */
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        /* When the input pointer is non‑NULL the cursor must reference the same memory */
        assert(cur.ptr == (uint8_t *)c_str);
        /* Length must equal the number of characters before the terminating NUL */
        assert(cur.len == str_len);
    }

    /* 8. The cursor must satisfy its validity predicate */
    assert(aws_byte_cursor_is_valid(&cur));
}
