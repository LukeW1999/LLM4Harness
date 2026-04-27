#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

void aws_byte_cursor_from_c_str_harness() {
    /* 1. Declare and bound data structures */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    const char *c_str = (const char *)malloc(len + 1);
    if (c_str) {
        for (size_t i = 0; i < len; i++) {
            c_str[i] = nondet_uint8_t();
        }
        c_str[len] = '\0'; // Null-terminate the string
    }

    /* 2. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 3. Assert postconditions for BOTH success and failure paths */
    if (c_str) {
        // Success path: c_str is not NULL
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == strlen(c_str));
    } else {
        // Failure path: c_str is NULL
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    }

    /* 4. Assert validity invariant always holds */
    assert(aws_byte_cursor_is_valid(&cur));

    free((char *)c_str);
}
