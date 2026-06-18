#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Nondeterministically create a C string (or NULL) */
    const char *c_str;
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t max_len = MAX_BUFFER_SIZE;               /* bound for the string length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len < max_len);               /* ensure we stay within the bound */

        char *buf = malloc(len + 1);                    /* allocate space for the string + NUL */
        __CPROVER_assume(buf != NULL);                 /* allocation must succeed for the harness */

        /* fill the allocated buffer with nondeterministic bytes */
        for (size_t i = 0; i < len; ++i) {
            buf[i] = nondet_uint8_t();
        }
        buf[len] = '\0';                               /* NUL‑terminate the string */

        c_str = buf;
    }

    /* 2. Save the original contents of the string (if any) */
    struct store_byte_from_buffer old_bytes;
    size_t old_len = 0;
    if (c_str) {
        old_len = strlen(c_str);
        save_byte_from_array((const uint8_t *)c_str, old_len, &old_bytes);
    }

    /* 3. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 4. Assert post‑conditions */
    assert(cur.ptr == (const uint8_t *)c_str);
    if (c_str) {
        assert(cur.len == strlen(c_str));
    } else {
        assert(cur.len == 0);
    }

    /* 5. The input string must remain unchanged */
    if (c_str) {
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_bytes);
    }

    /* 6. The returned cursor must satisfy its validity invariant */
    assert(aws_byte_cursor_is_valid(&cur));
}
