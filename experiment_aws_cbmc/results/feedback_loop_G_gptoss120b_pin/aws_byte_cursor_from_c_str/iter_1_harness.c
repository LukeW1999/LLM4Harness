#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Nondeterministically choose a NULL or a valid C string */
    const char *c_str;
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len < max_len);
        char *buf = malloc(len + 1);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < len; ++i) {
            buf[i] = (char)nondet_uint8_t();
        }
        buf[len] = '\0';
        c_str = buf;
    }

    /* 2. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 3. Compute the expected length */
    size_t expected_len = 0;
    if (c_str != NULL) {
        expected_len = strlen(c_str);
    } else {
        expected_len = 0;
    }

    /* 4. Post‑condition asserts */
    assert(cur.ptr == (uint8_t *)c_str);
    assert(cur.len == expected_len);
    assert(aws_byte_cursor_is_valid(&cur));

    /* 5. Clean up */
    if (c_str != NULL) {
        free((void *)c_str);
    }
}
